//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_executor.cpp
//
// Identification: src/execution/seq_scan_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/seq_scan_executor.h"

namespace bustub {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan)
    : AbstractExecutor(exec_ctx), plan_(plan) {}

void SeqScanExecutor::Init() {
  auto *exec_ctx = GetExecutorContext();
  auto *catalog = exec_ctx->GetCatalog();
  auto table_oid = plan_->GetTableOid();
  auto *table_metadata = catalog->GetTable(table_oid);
  table_heap_ = table_metadata->table_.get();
  table_iter_ = table_heap_->Begin(exec_ctx->GetTransaction());
  txn_ = GetExecutorContext()->GetTransaction();
  lock_manager_ = GetExecutorContext()->GetLockManager();

  try {
    if (txn_->GetIsolationLevel() != IsolationLevel::READ_UNCOMMITTED) {
      if (!lock_manager_->LockTable(txn_, LockManager::LockMode::INTENTION_SHARED, plan_->GetTableOid())) {
        throw ExecutionException("Fail to lock table");
      }
    }
  } catch (TransactionAbortException &e) {
    throw ExecutionException("Fail to lock table");
  }
}

auto SeqScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  auto table_oid = plan_->GetTableOid();
  if (table_iter_ == table_heap_->End()) {
    if (txn_->GetIsolationLevel() == IsolationLevel::READ_COMMITTED) {
      auto shared_row_lock_set = txn_->GetSharedRowLockSet()->at(table_oid);
      for (auto rid : shared_row_lock_set) {
        lock_manager_->UnlockRow(txn_, table_oid, rid);
      }
      lock_manager_->UnlockTable(txn_, plan_->GetTableOid());
    }
    return false;
  }

  *rid = table_iter_->GetRid();
  try {
    if (txn_->GetIsolationLevel() != IsolationLevel::READ_UNCOMMITTED) {
      if (!lock_manager_->LockRow(txn_, LockManager::LockMode::SHARED, table_oid, *rid)) {
        txn_->SetState(TransactionState::ABORTED);
        throw ExecutionException("Fail to lock row");
      }
    }
  } catch (TransactionAbortException &e) {
    throw ExecutionException("Fail to lock row");
  }

  std::vector<Value> values{};
  auto value_num = GetOutputSchema().GetColumnCount();
  values.reserve(value_num);

  for (uint32_t i = 0; i < value_num; i++) {
    values.push_back(table_iter_->GetValue(&GetOutputSchema(), i));
  }
  *tuple = Tuple{values, &GetOutputSchema()};

  table_iter_++;
  return true;
}

}  // namespace bustub
