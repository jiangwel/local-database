/**
 * deadlock_detection_test.cpp
 */

#include <atomic>
#include <random>
#include <thread>  // NOLINT

#include "common/config.h"
#include "concurrency/lock_manager.h"
#include "concurrency/transaction_manager.h"
#include "gtest/gtest.h"
#define TEST_TIMEOUT_BEGIN                           \
  std::promise<bool> promisedFinished;               \
  auto futureResult = promisedFinished.get_future(); \
                              std::thread([](std::promise<bool>& finished) {
#define TEST_TIMEOUT_FAIL_END(X)                                                                  \
  finished.set_value(true);                                                                       \
  }, std::ref(promisedFinished)).detach();                                                        \
  EXPECT_TRUE(futureResult.wait_for(std::chrono::milliseconds(X)) != std::future_status::timeout) \
      << "Test Failed Due to Time Out";

namespace bustub {
TEST(LockManagerDeadlockDetectionTest, DISABLED_EdgeTest) {
  LockManager lock_mgr{};

  const int num_nodes = 100;
  const int num_edges = num_nodes / 2;
  const int seed = 15445;
  std::srand(seed);

  // Create txn ids and shuffle
  std::vector<txn_id_t> txn_ids;
  txn_ids.reserve(num_nodes);
  for (int i = 0; i < num_nodes; i++) {
    txn_ids.push_back(i);
  }
  EXPECT_EQ(num_nodes, txn_ids.size());
  auto rng = std::default_random_engine{};
  std::shuffle(txn_ids.begin(), txn_ids.end(), rng);
  EXPECT_EQ(num_nodes, txn_ids.size());

  // Create edges by pairing adjacent txn_ids
  std::vector<std::pair<txn_id_t, txn_id_t>> edges;
  for (int i = 0; i < num_nodes; i += 2) {
    EXPECT_EQ(i / 2, lock_mgr.GetEdgeList().size());
    auto t1 = txn_ids[i];
    auto t2 = txn_ids[i + 1];
    lock_mgr.AddEdge(t1, t2);
    edges.emplace_back(t1, t2);
    EXPECT_EQ((i / 2) + 1, lock_mgr.GetEdgeList().size());
  }

  auto lock_mgr_edges = lock_mgr.GetEdgeList();
  EXPECT_EQ(num_edges, lock_mgr_edges.size());
  EXPECT_EQ(num_edges, edges.size());

  std::sort(lock_mgr_edges.begin(), lock_mgr_edges.end());
  std::sort(edges.begin(), edges.end());

  for (int i = 0; i < num_edges; i++) {
    EXPECT_EQ(edges[i], lock_mgr_edges[i]);
  }
}

TEST(LockManagerDeadlockDetectionTest, BasicDeadlockDetectionTest) {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t toid{0};
  RID rid0{0, 0};
  RID rid1{1, 1};
  auto *txn0 = txn_mgr.Begin();
  auto *txn1 = txn_mgr.Begin();
  EXPECT_EQ(0, txn0->GetTransactionId());
  EXPECT_EQ(1, txn1->GetTransactionId());

  std::thread t0([&] {
    // Lock and sleep
    bool res = lock_mgr.LockTable(txn0, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    EXPECT_EQ(true, res);
    // txn0,t1,r0
    res = lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid0);
    EXPECT_EQ(true, res);
    EXPECT_EQ(TransactionState::GROWING, txn1->GetState());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // This will block
    // txn0,t1,r1 txn0-->txn1
    res = lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid1);
    EXPECT_EQ(true, res);

    lock_mgr.UnlockRow(txn0, toid, rid1);
    lock_mgr.UnlockRow(txn0, toid, rid0);
    lock_mgr.UnlockTable(txn0, toid);

    txn_mgr.Commit(txn0);
    EXPECT_EQ(TransactionState::COMMITTED, txn0->GetState());
  });

  std::thread t1([&] {
    // Sleep so T0 can take necessary locks
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    bool res = lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    EXPECT_EQ(res, true);
    // txn1,t1,r1
    res = lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid1);
    EXPECT_EQ(TransactionState::GROWING, txn1->GetState());

    // This will block
    // txn1,t1,r0 txn1-->txn0
    res = lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid0);
    EXPECT_EQ(res, false);

    EXPECT_EQ(TransactionState::ABORTED, txn1->GetState());
    txn_mgr.Abort(txn1);
  });

  // Sleep for enough time to break cycle
  std::this_thread::sleep_for(cycle_detection_interval * 2);

  t0.join();
  t1.join();

  delete txn0;
  delete txn1;
}

TEST(LockManagerDeadlockDetectionTest, BlockTest) {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t toid{0};

  auto *txn0 = txn_mgr.Begin();
  auto *txn1 = txn_mgr.Begin();
  auto *txn2 = txn_mgr.Begin();

  std::thread t0([&] { lock_mgr.LockTable(txn0, LockManager::LockMode::EXCLUSIVE, toid); });
  std::thread t1([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, toid);
  });
  std::thread t2([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    lock_mgr.LockTable(txn2, LockManager::LockMode::EXCLUSIVE, toid);
  });

  t0.join();
  t1.join();
  t2.join();

  delete txn0;
  delete txn1;
  delete txn2;
}

TEST(LockManagerDeadlockDetectionTest, DISABLED_BasicDeadlockDetectionTest2) {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t toid{0};
  RID rid0{0, 0};
  RID rid1{1, 1};
  RID rid2{2, 2};
  RID rid3{3, 3};
  RID rid4{4, 4};
  RID rid5{5, 5};
  auto *txn0 = txn_mgr.Begin();
  auto *txn1 = txn_mgr.Begin();
  auto *txn2 = txn_mgr.Begin();
  auto *txn3 = txn_mgr.Begin();
  auto *txn4 = txn_mgr.Begin();
  auto *txn5 = txn_mgr.Begin();

  std::thread t0([&] {
    lock_mgr.LockTable(txn0, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid0);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid1);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    lock_mgr.UnlockRow(txn0, toid, rid0);
    lock_mgr.UnlockRow(txn0, toid, rid1);
  });

  std::thread t1([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid1);
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid2);
    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    lock_mgr.UnlockRow(txn1, toid, rid1);
    lock_mgr.UnlockRow(txn1, toid, rid2);
    lock_mgr.UnlockTable(txn1, toid);
  });
  std::thread t2([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn2, LockManager::LockMode::EXCLUSIVE, toid, rid2);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    lock_mgr.LockRow(txn2, LockManager::LockMode::EXCLUSIVE, toid, rid0);
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    lock_mgr.UnlockTable(txn2, toid);
  });
  std::thread t3([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    lock_mgr.LockTable(txn3, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn3, LockManager::LockMode::EXCLUSIVE, toid, rid3);
    std::this_thread::sleep_for(std::chrono::milliseconds(450));
    lock_mgr.LockRow(txn3, LockManager::LockMode::EXCLUSIVE, toid, rid4);
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    lock_mgr.UnlockRow(txn3, toid, rid3);
    lock_mgr.UnlockRow(txn3, toid, rid4);
    lock_mgr.UnlockTable(txn3, toid);
  });
  std::thread t4([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    lock_mgr.LockTable(txn4, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn4, LockManager::LockMode::EXCLUSIVE, toid, rid4);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lock_mgr.LockRow(txn4, LockManager::LockMode::EXCLUSIVE, toid, rid5);
    std::this_thread::sleep_for(std::chrono::milliseconds(700));
    lock_mgr.UnlockRow(txn4, toid, rid4);
    lock_mgr.UnlockRow(txn4, toid, rid5);
    lock_mgr.UnlockTable(txn4, toid);
  });
  std::thread t5([&] {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    lock_mgr.LockTable(txn5, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
    lock_mgr.LockRow(txn5, LockManager::LockMode::EXCLUSIVE, toid, rid5);
    std::this_thread::sleep_for(std::chrono::milliseconds(550));
    lock_mgr.LockRow(txn5, LockManager::LockMode::EXCLUSIVE, toid, rid3);
    lock_mgr.UnlockRow(txn5, toid, rid5);
    lock_mgr.UnlockTable(txn5, toid);
  });

  /*
  lt(t0,x,0)
  lr(t0,x,0,0)
  lt(t1,x,0)
  lr(t1,x,0,1)
  lt(t2,x,0)
  lr(t2,x,0,2)
  lt(t3,x,0)
  lr(t3,x,0,3)
  lr(t0,x,0,1)
  lr(t2,x,0,0)
  lr(t1,x,0,2)
  lr(t3,x,0,2)
  unlr(t2,0,2)
  */
  lock_mgr.LockTable(txn0, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
  lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid0);
  lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
  lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid1);
  lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
  lock_mgr.LockRow(txn2, LockManager::LockMode::EXCLUSIVE, toid, rid2);
  lock_mgr.LockTable(txn3, LockManager::LockMode::INTENTION_EXCLUSIVE, toid);
  lock_mgr.LockRow(txn3, LockManager::LockMode::EXCLUSIVE, toid, rid3);
  lock_mgr.LockRow(txn0, LockManager::LockMode::EXCLUSIVE, toid, rid1);
  lock_mgr.LockRow(txn2, LockManager::LockMode::EXCLUSIVE, toid, rid0);
  lock_mgr.LockRow(txn1, LockManager::LockMode::EXCLUSIVE, toid, rid2);
  lock_mgr.LockRow(txn3, LockManager::LockMode::EXCLUSIVE, toid, rid2);
  lock_mgr.UnlockRow(txn2, toid, rid2);

  delete txn0;
  delete txn1;
  delete txn2;
  delete txn3;
  delete txn4;
  delete txn5;
}
}  // namespace bustub
