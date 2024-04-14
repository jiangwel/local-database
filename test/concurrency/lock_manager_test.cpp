/**
 * lock_manager_test.cpp
 */

#include "concurrency/lock_manager.h"

#include <random>
#include <thread>  // NOLINT

#include "common/config.h"
#include "concurrency/transaction_manager.h"
#include "gtest/gtest.h"

namespace bustub {

/*
 * This test is only a sanity check. Please do not rely on this test
 * to check the correctness.
 */

// --- Helper functions ---
void CheckGrowing(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::GROWING); }

void CheckShrinking(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::SHRINKING); }

void CheckAborted(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::ABORTED); }

void CheckCommitted(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::COMMITTED); }

void CheckTxnRowLockSize(Transaction *txn, table_oid_t oid, size_t shared_size, size_t exclusive_size) {
  EXPECT_EQ((*(txn->GetSharedRowLockSet()))[oid].size(), shared_size);
  EXPECT_EQ((*(txn->GetExclusiveRowLockSet()))[oid].size(), exclusive_size);
}

int GetTxnTableLockSize(Transaction *txn, LockManager::LockMode lock_mode) {
  switch (lock_mode) {
    case LockManager::LockMode::SHARED:
      return txn->GetSharedTableLockSet()->size();
    case LockManager::LockMode::EXCLUSIVE:
      return txn->GetExclusiveTableLockSet()->size();
    case LockManager::LockMode::INTENTION_SHARED:
      return txn->GetIntentionSharedTableLockSet()->size();
    case LockManager::LockMode::INTENTION_EXCLUSIVE:
      return txn->GetIntentionExclusiveTableLockSet()->size();
    case LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE:
      return txn->GetSharedIntentionExclusiveTableLockSet()->size();
  }

  return -1;
}

void CheckTableLockSizes(Transaction *txn, size_t s_size, size_t x_size, size_t is_size, size_t ix_size,
                         size_t six_size) {
  EXPECT_EQ(s_size, txn->GetSharedTableLockSet()->size());
  EXPECT_EQ(x_size, txn->GetExclusiveTableLockSet()->size());
  EXPECT_EQ(is_size, txn->GetIntentionSharedTableLockSet()->size());
  EXPECT_EQ(ix_size, txn->GetIntentionExclusiveTableLockSet()->size());
  EXPECT_EQ(six_size, txn->GetSharedIntentionExclusiveTableLockSet()->size());
}
// test functions
void TableLockTest1() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  std::vector<table_oid_t> oids;
  std::vector<Transaction *> txns;

  /** 10 tables */
  int num_oids = 10;
  for (int i = 0; i < num_oids; i++) {
    table_oid_t oid{static_cast<uint32_t>(i)};
    oids.push_back(oid);
    txns.push_back(txn_mgr.Begin());
    EXPECT_EQ(i, txns[i]->GetTransactionId());
  }

  /** Each transaction takes an S lock on every table and then unlocks */
  auto task = [&](int txn_id) {
    bool res;
    for (const table_oid_t &oid : oids) {
      res = lock_mgr.LockTable(txns[txn_id], LockManager::LockMode::EXCLUSIVE, oid);
      EXPECT_TRUE(res);
      CheckGrowing(txns[txn_id]);
    }
    for (const table_oid_t &oid : oids) {
      res = lock_mgr.UnlockTable(txns[txn_id], oid);
      EXPECT_TRUE(res);
      CheckShrinking(txns[txn_id]);
    }
    txn_mgr.Commit(txns[txn_id]);
    CheckCommitted(txns[txn_id]);

    /** All locks should be dropped */
    CheckTableLockSizes(txns[txn_id], 0, 0, 0, 0, 0);
  };

  std::vector<std::thread> threads;
  threads.reserve(num_oids);

  for (int i = 0; i < num_oids; i++) {
    threads.emplace_back(std::thread{task, i});
  }

  for (int i = 0; i < num_oids; i++) {
    threads[i].join();
  }

  for (int i = 0; i < num_oids; i++) {
    delete txns[i];
  }
}

void WaitTest() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  std::vector<table_oid_t> oids;
  std::vector<Transaction *> txns;

  /** 10 tables */
  int num_oids = 10;
  for (int i = 0; i < num_oids; i++) {
    table_oid_t oid{static_cast<uint32_t>(i)};
    oids.push_back(oid);
    txns.push_back(txn_mgr.Begin());
    EXPECT_EQ(i, txns[i]->GetTransactionId());
  }

  table_oid_t oid{static_cast<uint32_t>(1)};
  /** Each transaction takes an S lock on every table and then unlocks */
  auto task = [&](int txn_id) {
    bool res;
    res = lock_mgr.LockTable(txns[txn_id], LockManager::LockMode::EXCLUSIVE, oid);
    EXPECT_TRUE(res);
    CheckGrowing(txns[txn_id]);

    res = lock_mgr.UnlockTable(txns[txn_id], oid);
    EXPECT_TRUE(res);
    CheckShrinking(txns[txn_id]);
    txn_mgr.Commit(txns[txn_id]);
    CheckCommitted(txns[txn_id]);

    /** All locks should be dropped */
    CheckTableLockSizes(txns[txn_id], 0, 0, 0, 0, 0);
  };

  std::vector<std::thread> threads;
  threads.reserve(num_oids);

  for (int i = 0; i < num_oids; i++) {
    threads.emplace_back(std::thread{task, i});
  }

  for (int i = 0; i < num_oids; i++) {
    threads[i].join();
  }

  for (int i = 0; i < num_oids; i++) {
    delete txns[i];
  }
}

void MemaryLackTest() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  std::vector<table_oid_t> oids;
  std::vector<Transaction *> txns;
  table_oid_t oid = 0;
  int num_oids = 1;
  for (int i = 0; i < num_oids; i++) {
    txns.push_back(txn_mgr.Begin());
    EXPECT_EQ(i, txns[i]->GetTransactionId());
  }

  /** Each transaction takes an S lock on every table and then unlocks */
  auto task = [&](int txn_id) {
    bool res;
    res = lock_mgr.LockTable(txns[txn_id], LockManager::LockMode::EXCLUSIVE, oid);
    EXPECT_TRUE(res);
    CheckGrowing(txns[txn_id]);
    res = lock_mgr.UnlockTable(txns[txn_id], oid);
    EXPECT_TRUE(res);
    CheckShrinking(txns[txn_id]);
    txn_mgr.Commit(txns[txn_id]);
    CheckCommitted(txns[txn_id]);

    /** All locks should be dropped */
    CheckTableLockSizes(txns[txn_id], 0, 0, 0, 0, 0);
  };

  std::vector<std::thread> threads;
  threads.reserve(num_oids);

  for (int i = 0; i < num_oids; i++) {
    threads.emplace_back(std::thread{task, i});
  }

  for (int i = 0; i < num_oids; i++) {
    threads[i].join();
  }

  for (int i = 0; i < num_oids; i++) {
    delete txns[i];
  }
}
/** Upgrading single transaction from S -> X */
void TableLockUpgradeTest1() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t oid = 0;
  auto txn1 = txn_mgr.Begin();

  /** Take S lock */
  EXPECT_EQ(true, lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid));
  CheckTableLockSizes(txn1, 1, 0, 0, 0, 0);

  /** Upgrade S to X */
  EXPECT_EQ(true, lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid));
  CheckTableLockSizes(txn1, 0, 1, 0, 0, 0);

  /** Clean up */
  txn_mgr.Commit(txn1);
  CheckCommitted(txn1);
  CheckTableLockSizes(txn1, 0, 0, 0, 0, 0);

  delete txn1;
}

void RowLockTest1() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t oid = 0;
  RID rid{0, 0};

  int num_txns = 3;
  std::vector<Transaction *> txns;
  for (int i = 0; i < num_txns; i++) {
    txns.push_back(txn_mgr.Begin());
    EXPECT_EQ(i, txns[i]->GetTransactionId());
  }

  /** Each transaction takes an S lock on the same table and row and then unlocks */
  auto task = [&](int txn_id) {
    bool res;

    res = lock_mgr.LockTable(txns[txn_id], LockManager::LockMode::SHARED, oid);
    EXPECT_TRUE(res);
    CheckGrowing(txns[txn_id]);

    res = lock_mgr.LockRow(txns[txn_id], LockManager::LockMode::SHARED, oid, rid);
    EXPECT_TRUE(res);
    CheckGrowing(txns[txn_id]);
    /** Lock set should be updated */
    ASSERT_EQ(true, txns[txn_id]->IsRowSharedLocked(oid, rid));

    res = lock_mgr.UnlockRow(txns[txn_id], oid, rid);
    EXPECT_TRUE(res);
    CheckShrinking(txns[txn_id]);
    /** Lock set should be updated */
    ASSERT_EQ(false, txns[txn_id]->IsRowSharedLocked(oid, rid));

    res = lock_mgr.UnlockTable(txns[txn_id], oid);
    EXPECT_TRUE(res);
    CheckShrinking(txns[txn_id]);

    txn_mgr.Commit(txns[txn_id]);
    CheckCommitted(txns[txn_id]);
  };

  std::vector<std::thread> threads;
  threads.reserve(num_txns);

  for (int i = 0; i < num_txns; i++) {
    threads.emplace_back(std::thread{task, i});
  }

  for (int i = 0; i < num_txns; i++) {
    threads[i].join();
    delete txns[i];
  }
}

void TwoPLTest1() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};
  table_oid_t oid = 0;

  RID rid0{0, 0};
  RID rid1{0, 1};

  auto *txn = txn_mgr.Begin();
  EXPECT_EQ(0, txn->GetTransactionId());

  bool res;
  res = lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid);
  EXPECT_TRUE(res);
  // lock s rid0
  res = lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid0);
  EXPECT_TRUE(res);

  CheckGrowing(txn);
  CheckTxnRowLockSize(txn, oid, 1, 0);
  // lock x rid1
  res = lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid1);
  EXPECT_TRUE(res);
  CheckGrowing(txn);
  CheckTxnRowLockSize(txn, oid, 1, 1);
  // unlock s rid0
  res = lock_mgr.UnlockRow(txn, oid, rid0);
  EXPECT_TRUE(res);
  CheckShrinking(txn);
  CheckTxnRowLockSize(txn, oid, 0, 1);

  try {
    // lock s rid0
    lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid0);
  } catch (TransactionAbortException &e) {
    CheckAborted(txn);
    CheckTxnRowLockSize(txn, oid, 0, 1);
  }

  // Need to call txn_mgr's abort
  txn_mgr.Abort(txn);
  CheckAborted(txn);
  CheckTxnRowLockSize(txn, oid, 0, 0);
  CheckTableLockSizes(txn, 0, 0, 0, 0, 0);

  delete txn;
}

void MultiTableMultiTxnSLockTest() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  const int num_tables = 5;
  const int num_txns = 3;
  std::vector<table_oid_t> oids(num_tables);
  std::vector<Transaction *> txns(num_txns);

  // 初始化表和事务
  for (int i = 0; i < num_tables; i++) {
    oids[i] = static_cast<table_oid_t>(i);
  }

  for (int i = 0; i < num_txns; i++) {
    txns[i] = txn_mgr.Begin();
    EXPECT_EQ(i, txns[i]->GetTransactionId());
  }

  // 每个事务对每个表获取S锁
  for (int i = 0; i < num_txns; i++) {
    for (const auto &oid : oids) {
      bool res = lock_mgr.LockTable(txns[i], LockManager::LockMode::SHARED, oid);
      EXPECT_TRUE(res);
      CheckGrowing(txns[i]);
    }
    CheckTableLockSizes(txns[i], num_tables, 0, 0, 0, 0);
  }

  // 每个事务解锁每个表
  for (int i = 0; i < num_txns; i++) {
    for (const auto &oid : oids) {
      bool res = lock_mgr.UnlockTable(txns[i], oid);
      EXPECT_TRUE(res);
      CheckShrinking(txns[i]);
    }
    CheckTableLockSizes(txns[i], 0, 0, 0, 0, 0);
    txn_mgr.Commit(txns[i]);
    CheckCommitted(txns[i]);
  }

  // 清理
  for (auto txn : txns) {
    delete txn;
  }
}

// Exception tests
void AttemptedIntentionLockOnRow() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin();

  // 尝试对行获取意图锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::INTENTION_SHARED, oid, rid), TransactionAbortException);
  txn->SetState(TransactionState::GROWING);
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid, rid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}

void LockOnShrinking() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin(nullptr, IsolationLevel::READ_COMMITTED);

  // 获取共享锁使事务进入GROWING状态
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));

  // 解锁使事务进入SHRINKING状态
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // 在SHRINKING状态下尝试获取排他锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}
void TableLockNotPresent() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin();

  // 尝试对行获取排他锁而没有表锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}

void UpgradeConflict() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn1 = txn_mgr.Begin();
  auto *txn2 = txn_mgr.Begin();

  // 两个事务都获取共享锁
  EXPECT_TRUE(lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn2, LockManager::LockMode::SHARED, oid));

  // 一个事务尝试将共享锁升级为排他锁
  EXPECT_TRUE(lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid));

  // 另一个事务也尝试升级,应该抛出异常
  EXPECT_THROW(lock_mgr.LockTable(txn2, LockManager::LockMode::EXCLUSIVE, oid), TransactionAbortException);

  txn_mgr.Abort(txn1);
  txn_mgr.Abort(txn2);
  delete txn1;
  delete txn2;
}
void AttemptedUnlockButNoLockHeld() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin();

  // 尝试解锁未获取的锁,应该抛出异常
  EXPECT_THROW(lock_mgr.UnlockTable(txn, oid), TransactionAbortException);
  EXPECT_THROW(lock_mgr.UnlockRow(txn, oid, rid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}
void TableUnlockedBeforeUnlockingRows() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin();

  // 获取表和行锁
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid));

  // 尝试先解锁表而不解锁行,应该抛出异常
  EXPECT_THROW(lock_mgr.UnlockTable(txn, oid), TransactionAbortException);
  delete txn;
}
// 测试表锁的兼容性
class TableLockCompatibilityTest : public ::testing::Test {
 protected:
  void SetUp() override {
    lock_mgr = std::make_unique<LockManager>();
    txn_mgr = std::make_unique<TransactionManager>(lock_mgr.get());
    table_oid = 1;
  }

  std::unique_ptr<LockManager> lock_mgr;
  std::unique_ptr<TransactionManager> txn_mgr;
  table_oid_t table_oid;
};
void TestLockCompatibility() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  table_oid_t table1_oid = 1;

  // 事务a获取表1上的S锁
  auto *txn_a = txn_mgr.Begin();
  EXPECT_TRUE(lock_mgr.LockTable(txn_a, LockManager::LockMode::SHARED, table1_oid));

  // 事务b获取表1上的S锁,应该成功
  auto *txn_b = txn_mgr.Begin();
  EXPECT_TRUE(lock_mgr.LockTable(txn_b, LockManager::LockMode::SHARED, table1_oid));

  // // 事务c试图获取表1上的X锁,应该失败
  // auto *txn_c = txn_mgr.Begin();
  // EXPECT_FALSE(lock_mgr.LockTable(txn_c, LockManager::LockMode::EXCLUSIVE, table1_oid));

  delete txn_a;
  delete txn_b;
  // delete txn_c;
}

// Lock upgrade tests
void TableLockUpgradeTest() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn = txn_mgr.Begin();

  // IS -> S (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // IS -> X (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // IS -> IX (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // IS -> SIX (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // S -> X (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // S -> SIX (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // IX -> X (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // IX -> SIX (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // SIX -> X (不解锁)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));

  txn_mgr.Commit(txn);
  delete txn;
}
void RowLockUpgradeTest() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin();

  // 获取表锁以支持行锁升级
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid));

  // S -> X (不解锁)
  EXPECT_TRUE(lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid));
  EXPECT_TRUE(lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid));

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockRow(txn, oid, rid));

  // X -> S (不允许降级)
  EXPECT_TRUE(lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid));
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid), TransactionAbortException);

  // 解锁
  EXPECT_TRUE(lock_mgr.UnlockRow(txn, oid, rid));

  txn_mgr.Commit(txn);
  delete txn;
}
void LockUpgradeForSameLock() {
  LockManager lock_mgr{};
  TransactionManager txn_mgr{&lock_mgr};

  // Begin a transaction
  Transaction *txn = txn_mgr.Begin();
  EXPECT_EQ(0, txn->GetTransactionId());

  // Lock a table in shared mode
  table_oid_t oid = 1;
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));

  // Attempt to upgrade the lock to shared mode again
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));

  // Check the lock mode
  EXPECT_EQ(1, GetTxnTableLockSize(txn, LockManager::LockMode::SHARED));

  // Clean up
  txn_mgr.Commit(txn);
}
#define ENABLE_TESTING

#ifdef ENABLE_TESTING

TEST(LockManagerTest, MultiTableMultiTxnSLockTest) { MultiTableMultiTxnSLockTest(); }

TEST(LockManagerTest, TwoPLTest1) { TwoPLTest1(); }

TEST(LockManagerTest, RowLockTest1) { RowLockTest1(); }
TEST(LockManagerTest, MemaryLackTest) { MemaryLackTest(); }

TEST(LockManagerTest, ExceptionTest1) { AttemptedIntentionLockOnRow(); }

TEST(LockManagerTest, LockOnShrinking) { LockOnShrinking(); }
TEST(LockManagerTest, ExceptionTest4) { TableLockNotPresent(); }
TEST(LockManagerTest, DISABLED_UpgradeConflict) { UpgradeConflict(); }
TEST(LockManagerTest, ExceptionTest7) { AttemptedUnlockButNoLockHeld(); }

TEST(LockManagerTest, ExceptionTest8) { TableUnlockedBeforeUnlockingRows(); }

TEST(LockManagerTest, LockUpgradeToSameMode) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  const int num_tables = 5;
  std::vector<table_oid_t> table_oids;
  for (int i = 0; i < num_tables; i++) {
    table_oids.emplace_back(static_cast<table_oid_t>(i));
  }

  Transaction *txn = txn_mgr.Begin();

  // 获取 S 锁
  for (table_oid_t oid : table_oids) {
    EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  }

  // 尝试将 S 锁升级为 S 锁
  for (table_oid_t oid : table_oids) {
    EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  }

  txn_mgr.Commit(txn);
  delete txn;
}

TEST(LockManagerTest, TableLockTest1) { TableLockTest1(); }

TEST(LockManagerTest, IncompatibleTableLockUpgrade) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  Transaction *txn = txn_mgr.Begin();

  // IX -> IS (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  ++oid;
  txn = txn_mgr.Begin();

  // IX -> S (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  ++oid;
  txn = txn_mgr.Begin();

  // S -> IX (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  ++oid;
  txn = txn_mgr.Begin();

  // S -> IS (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  ++oid;
  txn = txn_mgr.Begin();

  // X -> IS (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  ++oid;
  txn = txn_mgr.Begin();

  // X -> S (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid), TransactionAbortException);
  txn_mgr.Abort(txn);
  delete txn;

  txn = txn_mgr.Begin();
  ++oid;

  // X -> IX (不允许)
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid));
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}
TEST(LockManagerTest, DISABLED_ConcurrentTableLockUpgrade) {
  // LockManager lock_mgr{};
  // TransactionManager txn_mgr{&lock_mgr};

  // Transaction* txn1 = txn_mgr.Begin();
  // // Transaction* txn2 = txn_mgr.Begin();

  // EXPECT_TRUE(lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, 1));
  // // EXPECT_TRUE(lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_SHARED, 1));

  // EXPECT_TRUE(lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, 1));

  // delete txn1;
  // // delete txn2;

  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  Transaction *txn = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  txn->SetState(TransactionState::GROWING);
  Transaction *txn2 = txn_mgr.Begin();

  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));
  EXPECT_TRUE(lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_SHARED, oid));

  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid));
  std::thread upgrade_thread([&] {
    EXPECT_THROW(lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, oid), TransactionAbortException);
  });

  upgrade_thread.join();
  // txn_mgr.Abort(txn);
  delete txn;
  delete txn2;
}
TEST(LockManagerTest, TestLockCompatibility) { TestLockCompatibility(); }
// 相容: IS + IS
TEST_F(TableLockCompatibilityTest, IntentionSharedCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::INTENTION_SHARED, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}
// 相容: IS + S
TEST_F(TableLockCompatibilityTest, IntentionSharedAndSharedCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::SHARED, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}
// 相容: S + S
TEST_F(TableLockCompatibilityTest, SharedCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::SHARED, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::SHARED, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}

// 兼容: IS + IX
TEST_F(TableLockCompatibilityTest, IntentionSharedAndIntentionExclusiveCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}

// 兼容: IS + SIX
TEST_F(TableLockCompatibilityTest, IntentionSharedAndSharedIntentionExclusiveCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}

// 兼容: IX + IX
TEST_F(TableLockCompatibilityTest, IntentionExclusiveCompatible) {
  auto *txn1 = txn_mgr->Begin();
  auto *txn2 = txn_mgr->Begin();

  EXPECT_TRUE(lock_mgr->LockTable(txn1, LockManager::LockMode::INTENTION_EXCLUSIVE, table_oid));
  EXPECT_TRUE(lock_mgr->LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, table_oid));

  txn_mgr->Commit(txn1);
  txn_mgr->Commit(txn2);
  delete txn1;
  delete txn2;
}

TEST(LockManagerTest, DISABLED_blocktest) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn1 = txn_mgr.Begin();
  auto *txn2 = txn_mgr.Begin();
  auto *txn3 = txn_mgr.Begin();
  lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
  lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_SHARED, oid);
  lock_mgr.LockTable(txn3, LockManager::LockMode::INTENTION_EXCLUSIVE, oid);
  EXPECT_EQ(txn3->GetIntentionExclusiveTableLockSet()->size(), 0);

  delete txn1;
  delete txn2;
  delete txn3;
}

TEST(LockManagerTest, DISABLED_RepeatableReadTest) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn1 = txn_mgr.Begin();
  lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn1->SetState(TransactionState::GROWING);
  lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn1->SetState(TransactionState::GROWING);
  lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn1->SetState(TransactionState::GROWING);
  lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_EXCLUSIVE, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn1->SetState(TransactionState::GROWING);
  lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn1->SetState(TransactionState::GROWING);
  lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid);
  lock_mgr.UnlockTable(txn1, oid);
  lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
  CheckCommitted(txn1);
  delete txn1;
}
TEST(LockManager, WaitTest) { WaitTest(); }
TEST(LockManagerTest, CompatibleUpgradeTest) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn1 = txn_mgr.Begin();
  lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
  lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid);
  lock_mgr.UnlockTable(txn1, oid);
  txn_mgr.Commit(txn1);
  CheckCommitted(txn1);
  delete txn1;
}
#endif

TEST(LockManagerTest, DISABLED_TableLockUpgrade) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);
  table_oid_t oid = 0;
  int i = 0;

  auto *txn0 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn1 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn2 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lck(mtx);

  std::thread t0([&] {
    lock_mgr.LockTable(txn0, LockManager::LockMode::SHARED, oid);
    lock_mgr.LockTable(txn0, LockManager::LockMode::EXCLUSIVE, oid);
    lock_mgr.UnlockTable(txn0, oid);
    txn0->SetState(TransactionState::GROWING);
    i = 1;
    cv.notify_all();
    cv.wait(lck, [&] { return i == 3; });
    std::cout << 3 << std::endl;

    i = 4;
    cv.notify_all();
    lock_mgr.LockTable(txn0, LockManager::LockMode::SHARED, oid);
    lock_mgr.LockTable(txn0, LockManager::LockMode::EXCLUSIVE, oid);
    cv.wait(lck, [&] { return i == 6; });
    std::cout << 6 << std::endl;
    lock_mgr.UnlockTable(txn0, oid);
    txn0->SetState(TransactionState::GROWING);
    lock_mgr.LockTable(txn0, LockManager::LockMode::SHARED, oid);
    i = 7;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 9; });
    std::cout << 9 << std::endl;
    lock_mgr.LockTable(txn0, LockManager::LockMode::EXCLUSIVE, oid);
    i = 10;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 100; });
  });
  std::thread t1([&] {
    cv.wait(lck, [&] { return i == 1; });
    std::cout << 1 << std::endl;
    lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
    i = 2;
    cv.notify_all();
    cv.wait(lck, [&] { return i == 4; });
    std::cout << 4 << std::endl;

    lock_mgr.UnlockTable(txn1, oid);
    EXPECT_EQ(0, txn0->GetSharedTableLockSet()->size());
    i = 5;
    cv.notify_all();
    cv.wait(lck, [&] { return i == 7; });
    std::cout << 7 << std::endl;
    txn1->SetState(TransactionState::GROWING);
    lock_mgr.LockTable(txn1, LockManager::LockMode::SHARED, oid);
    i = 8;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 10; });
    std::cout << 10 << std::endl;
    lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid);
    lock_mgr.UnlockTable(txn1, oid);
    i = 11;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 71; });
  });

  std::thread t2([&] {
    cv.wait(lck, [&] { return i == 2; });
    std::cout << 2 << std::endl;
    lock_mgr.LockTable(txn2, LockManager::LockMode::SHARED, oid);
    i = 3;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 5; });
    std::cout << 5 << std::endl;
    lock_mgr.UnlockTable(txn2, oid);
    i = 6;
    cv.notify_all();
    cv.wait(lck, [&] { return i == 8; });
    std::cout << 8 << std::endl;
    txn2->SetState(TransactionState::GROWING);
    lock_mgr.LockTable(txn2, LockManager::LockMode::SHARED, oid);
    i = 9;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 11; });
    std::cout << 11 << std::endl;
    lock_mgr.UnlockTable(txn2, oid);
    i = 12;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 71; });
  });

  t0.join();
  t1.join();
  t2.join();

  delete txn0;
  delete txn1;
  delete txn2;
}

TEST(LockManagerTest, DISABLED_Tableaborttest) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);
  table_oid_t oid = 0;
  int i = 0;

  auto *txn0 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn1 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn2 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lck(mtx);

  std::thread t0([&] {
    lock_mgr.LockTable(txn0, LockManager::LockMode::EXCLUSIVE, oid);
    i = 0;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 2; });
    lock_mgr.UnlockTable(txn0, oid);
    i = 3;
    cv.notify_all();
  });
  std::thread t1([&] {
    cv.wait(lck, [&] { return i == 0; });
    lock_mgr.LockTable(txn1, LockManager::LockMode::EXCLUSIVE, oid);
    i = 1;
    cv.notify_all();
    cv.wait(lck, [&] { return i == 4; });
  });

  std::thread t2([&] {
    cv.wait(lck, [&] { return i == 1; });
    lock_mgr.LockTable(txn2, LockManager::LockMode::EXCLUSIVE, oid);
    i = 2;
    cv.notify_all();

    cv.wait(lck, [&] { return i == 3; });
    EXPECT_FALSE(lock_mgr.LockTable(txn2, LockManager::LockMode::EXCLUSIVE, oid));
  });

  t0.join();
  t1.join();
  t2.join();

  delete txn0;
  delete txn1;
  delete txn2;
}

TEST(LockManagerTest, DISABLED_BlockedTest) {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);
  table_oid_t oid = 0;
  int i = 0;
  std::mutex mtx;
  std::condition_variable cv;
  std::unique_lock<std::mutex> lck(mtx);

  auto *txn0 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn1 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);
  auto *txn2 = txn_mgr.Begin(nullptr, IsolationLevel::REPEATABLE_READ);

  std::thread t0([&] {
    cv.wait(lck, [&] { return i == 1; });
    lock_mgr.UnlockTable(txn0, oid);
    EXPECT_EQ(txn2->GetIntentionExclusiveTableLockSet()->size(), 1);
  });
  std::thread t1([&] {
    lock_mgr.LockTable(txn0, LockManager::LockMode::SHARED, oid);
    lock_mgr.LockTable(txn1, LockManager::LockMode::INTENTION_SHARED, oid);

    i = 1;
    cv.notify_all();
    lock_mgr.LockTable(txn2, LockManager::LockMode::INTENTION_EXCLUSIVE, oid);
    EXPECT_EQ(txn2->GetIntentionExclusiveTableLockSet()->size(), 0);
  });
  t1.join();
  t0.join();

  delete txn0;
  delete txn1;
  delete txn2;
}
}  // namespace bustub
