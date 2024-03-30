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

  res = lock_mgr.LockRow(txn, LockManager::LockMode::SHARED, oid, rid0);
  EXPECT_TRUE(res);

  CheckGrowing(txn);
  CheckTxnRowLockSize(txn, oid, 1, 0);

  res = lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid1);
  EXPECT_TRUE(res);
  CheckGrowing(txn);
  CheckTxnRowLockSize(txn, oid, 1, 1);

  res = lock_mgr.UnlockRow(txn, oid, rid0);
  EXPECT_TRUE(res);
  CheckShrinking(txn);
  CheckTxnRowLockSize(txn, oid, 0, 1);

  try {
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
  std::vector<Transaction*> txns(num_txns);

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
    for (const auto& oid : oids) {
      bool res = lock_mgr.LockTable(txns[i], LockManager::LockMode::SHARED, oid);
      EXPECT_TRUE(res);
      CheckGrowing(txns[i]);
    }
    CheckTableLockSizes(txns[i], num_tables, 0, 0, 0, 0);
  }

  // 每个事务解锁每个表
  for (int i = 0; i < num_txns; i++) {
    for (const auto& oid : oids) {
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
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid, rid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}
void LockSharedOnReadUncommitted() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn = txn_mgr.Begin(nullptr,IsolationLevel::READ_UNCOMMITTED);

  // 尝试获取共享锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid), TransactionAbortException);
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid), TransactionAbortException);
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED_INTENTION_EXCLUSIVE, oid), TransactionAbortException);

  txn_mgr.Abort(txn);
  delete txn;
}
void LockOnShrinking() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  RID rid{0, 0};
  auto *txn = txn_mgr.Begin(nullptr,IsolationLevel::READ_COMMITTED);

  // 获取共享锁使事务进入GROWING状态
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::SHARED, oid));

  // 解锁使事务进入SHRINKING状态
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  // 在SHRINKING状态下尝试获取排他锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::EXCLUSIVE, oid), TransactionAbortException);
  EXPECT_THROW(lock_mgr.LockRow(txn, LockManager::LockMode::EXCLUSIVE, oid, rid), TransactionAbortException);

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
void IncompatibleUpgrade() {
  LockManager lock_mgr;
  TransactionManager txn_mgr(&lock_mgr);

  table_oid_t oid = 0;
  auto *txn = txn_mgr.Begin();

  // 获取意图共享锁
  EXPECT_TRUE(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_SHARED, oid));

  // 尝试将意图共享锁升级为意图排他锁,应该抛出异常
  EXPECT_THROW(lock_mgr.LockTable(txn, LockManager::LockMode::INTENTION_EXCLUSIVE, oid), TransactionAbortException);

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

  // 正确的解锁顺序
  EXPECT_TRUE(lock_mgr.UnlockRow(txn, oid, rid));
  EXPECT_TRUE(lock_mgr.UnlockTable(txn, oid));

  txn_mgr.Commit(txn);
  delete txn;
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

TEST(LockManagerTest, ExceptionTest) {
  AttemptedIntentionLockOnRow();
  LockSharedOnReadUncommitted();
  LockOnShrinking();
  TableLockNotPresent();
  IncompatibleUpgrade();
  UpgradeConflict();
  AttemptedUnlockButNoLockHeld();
  TableUnlockedBeforeUnlockingRows();
} 
}  // namespace bustub
