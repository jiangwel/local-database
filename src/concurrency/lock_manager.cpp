//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager.cpp
//
// Identification: src/concurrency/lock_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/lock_manager.h"

#include "common/config.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"

namespace bustub {
#define Debug
// #define DeadlockDetection
auto LockManager::LockTable(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("LockTable: txn_id: %d, lock_mode: %d, oid: %d,iso level: %d", txn->GetTransactionId(),
           static_cast<int>(lock_mode), oid, static_cast<int>(txn->GetIsolationLevel()));
#endif
  if (LockTableIllegalBehavior(txn, lock_mode, oid)) {
#ifdef Debug
    LOG_INFO("LockTableIllegalBehavior: txn_id: %d", txn->GetTransactionId());
#endif
    return false;
  }
  std::shared_ptr<LockRequestQueue> req_queue_ptr = nullptr;
  {
    std::lock_guard<std::mutex> guard(table_lock_map_latch_);
    auto req_queue_itr = table_lock_map_.find(oid);
    if (req_queue_itr == table_lock_map_.end()) {
      TableCreateNewReqQueue(txn, lock_mode, oid);
      TableTxnLockSetAddRecord(txn, lock_mode, oid);
      return true;
    }
    req_queue_ptr = req_queue_itr->second;
  }

  std::unique_lock<std::mutex> lck(req_queue_ptr->latch_);

  bool is_queue_empty = req_queue_ptr->request_queue_.empty();
  // if the queue is empty, grant the lock directly
  if (is_queue_empty) {
    auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
    new_req->granted_ = true;
    req_queue_ptr->request_queue_.emplace_back(new_req);
    TableTxnLockSetAddRecord(txn, lock_mode, oid);
    return true;
  }

  if (TryTableLockUpgrade(txn, req_queue_ptr, lock_mode)) {
    if (txn->GetState() == TransactionState::ABORTED) {
      return false;
    }
    TableTxnLockSetAddRecord(txn, lock_mode, oid);
    return true;
  }

  auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
  req_queue_ptr->request_queue_.push_back(new_req);

  while (!GrantLock(req_queue_ptr, new_req)) {
#ifdef Debug
    LOG_INFO("Wait Table Lock: txn_id: %d,oid: %d", txn->GetTransactionId(), oid);
#endif
    req_queue_ptr->cv_.wait(lck);
    if (txn->GetState() == TransactionState::ABORTED) {
      req_queue_ptr->request_queue_.remove(new_req);
      delete new_req;
      req_queue_ptr->cv_.notify_all();
      return false;
    }
#ifdef Debug
    LOG_INFO("Wait Table Lock Done: txn_id: %d", txn->GetTransactionId());
#endif
  }
  new_req->granted_ = true;
  TableTxnLockSetAddRecord(txn, lock_mode, oid);
  return true;
}

auto LockManager::UnlockTable(Transaction *txn, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("UnLockTable: txn_id: %d,oid: %d,iso level: %d", txn->GetTransactionId(), oid,
           static_cast<int>(txn->GetIsolationLevel()));
#endif
  if (UnlockTableIllegalBehavior(txn, oid)) {
#ifdef Debug
    LOG_INFO("UnlockTableIllegalBehavior: txn_id: %d", txn->GetTransactionId());
#endif
    return false;
  }
  // update lock table
  std::shared_ptr<LockRequestQueue> req_queue_ptr = nullptr;
  {
    std::lock_guard<std::mutex> guard(table_lock_map_latch_);
    auto req_queue_itr = table_lock_map_.find(oid);
    assert(req_queue_itr != table_lock_map_.end());
    req_queue_ptr = req_queue_itr->second;
  }

  std::lock_guard<std::mutex> guard(req_queue_ptr->latch_);
  LockRequest *txn_req = GetRequest(req_queue_ptr, txn->GetTransactionId());

  auto lock_mode = txn_req->lock_mode_;
  UpdateTxnState(txn, lock_mode);
  TableTxnLockSetDeleteRecord(txn, lock_mode, oid);

  auto *last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = !last_req->granted_ || req_queue_ptr->upgrading_ != INVALID_TXN_ID;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  if (has_wait_txn) {
    req_queue_ptr->cv_.notify_all();
  }
  return true;
}

auto LockManager::LockRow(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) -> bool {
#ifdef Debug
  LOG_INFO("LockRow: txn_id: %d, lock_mode: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(),
           static_cast<int>(lock_mode), oid, rid.ToString().c_str(), static_cast<int>(txn->GetIsolationLevel()));
#endif
  if (LockRowIllegalBehavior(txn, lock_mode, oid, rid)) {
#ifdef Debug
    LOG_INFO("LockRowIllegalBehavior: txn_id: %d", txn->GetTransactionId());
#endif
    return false;
  }
  std::shared_ptr<LockRequestQueue> req_queue_ptr = nullptr;
  {
    std::lock_guard<std::mutex> guard(row_lock_map_latch_);
    auto req_queue_itr = row_lock_map_.find(rid);
    if (req_queue_itr == row_lock_map_.end()) {
      RowCreateNewReqQueue(txn, lock_mode, oid, rid);
      RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
      return true;
    }
    req_queue_ptr = req_queue_itr->second;
  }

  std::unique_lock<std::mutex> lck(req_queue_ptr->latch_);

  bool is_queue_empty = req_queue_ptr->request_queue_.empty();
  if (is_queue_empty) {
    auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid);
    new_req->granted_ = true;
    req_queue_ptr->request_queue_.emplace_back(new_req);
    RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
    return true;
  }
  if (TryRowLockUpgrade(txn, req_queue_ptr, lock_mode)) {
    if (txn->GetState() == TransactionState::ABORTED) {
      return false;
    }
    RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
    return true;
  }

  auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid);
  req_queue_ptr->request_queue_.push_back(new_req);

  while (!GrantLock(req_queue_ptr, new_req)) {
#ifdef Debug
    LOG_INFO("Wait Row Lock: txn_id: %d,oid: %d", txn->GetTransactionId(), oid);
#endif
    req_queue_ptr->cv_.wait(lck);
    if (txn->GetState() == TransactionState::ABORTED) {
      req_queue_ptr->request_queue_.remove(new_req);
      delete new_req;
      req_queue_ptr->cv_.notify_all();
      return false;
    }
#ifdef Debug
    LOG_INFO("Wait Row Lock Done: txn_id: %d", txn->GetTransactionId());
#endif
  }

  new_req->granted_ = true;
  RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
  return true;
}

auto LockManager::UnlockRow(Transaction *txn, const table_oid_t &oid, const RID &rid) -> bool {
#ifdef Debug
  LOG_INFO("UnLockRow: txn_id: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(), oid, rid.ToString().c_str(),
           static_cast<int>(txn->GetIsolationLevel()));
#endif
  if (UnlockRowIllegalBehavior(txn, oid, rid)) {
#ifdef Debug
    LOG_INFO("UnlockRowIllegalBehavior: txn_id: %d", txn->GetTransactionId());
#endif
    return false;
  }
  // update lock table
  std::shared_ptr<LockRequestQueue> req_queue_ptr = nullptr;
  {
    std::lock_guard<std::mutex> guard(row_lock_map_latch_);
    auto req_queue_itr = row_lock_map_.find(rid);
    assert(req_queue_itr != row_lock_map_.end());
    req_queue_ptr = req_queue_itr->second;
  }

  std::lock_guard<std::mutex> guard(req_queue_ptr->latch_);
  LockRequest *txn_req = GetRequest(req_queue_ptr, txn->GetTransactionId());
  auto lock_mode = txn_req->lock_mode_;
#ifdef Debug
  LOG_INFO("lock_mode: %d", static_cast<int>(lock_mode));
#endif
  UpdateTxnState(txn, lock_mode);
  RowTxnLockSetDeleteRecord(txn, lock_mode, oid, rid);

  auto *last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = !last_req->granted_ || req_queue_ptr->upgrading_ != INVALID_TXN_ID;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  if (has_wait_txn) {
    req_queue_ptr->cv_.notify_all();
  }
  return true;
}

void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) {
  auto &neighbors = waits_for_[t1];
  auto it = std::find(neighbors.begin(), neighbors.end(), t2);

  if (it == neighbors.end()) {
    neighbors.push_back(t2);
  }
}

void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) {
  auto it = waits_for_.find(t1);
  if (it == waits_for_.end()) {
    return;
  }

  auto &neighbors = it->second;
  auto it_neighbor = std::find(neighbors.begin(), neighbors.end(), t2);
  if (it_neighbor != neighbors.end()) {
    neighbors.erase(it_neighbor);
  }
}

auto LockManager::HasCycle(txn_id_t *txn_id) -> bool {
  std::unordered_map<txn_id_t, bool> visited;
  std::vector<txn_id_t> start_nodes;

  for (const auto &pair : waits_for_) {
    start_nodes.push_back(pair.first);
  }
  std::sort(start_nodes.begin(), start_nodes.end());

  for (const txn_id_t &node : start_nodes) {
    if (visited.find(node) == visited.end()) {
      std::stack<txn_id_t> recursion_stack;
      recursion_stack.push(node);
      visited.emplace(node, true);
      if (DFS(node, visited, recursion_stack, *txn_id)) {
        return true;
      }
    }
  }

  return false;
}

auto LockManager::GetEdgeList() -> std::vector<std::pair<txn_id_t, txn_id_t>> {
  std::vector<std::pair<txn_id_t, txn_id_t>> edges(0);
  for (const auto &node_pair : waits_for_) {
    txn_id_t src_node = node_pair.first;
    const std::vector<txn_id_t> &neighbors = node_pair.second;

    for (const txn_id_t &dest_node : neighbors) {
      edges.emplace_back(src_node, dest_node);
    }
  }
  return edges;
}

void LockManager::RunCycleDetection() {
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(cycle_detection_interval);
#ifdef DeadlockDetection
    LOG_INFO("Start CycleDetection");
#endif
    {
      std::lock_guard<std::mutex> guard(waits_for_latch_);
      if (!enable_cycle_detection_) {
        break;
      }
      waits_for_.clear();
      BuildGraph();

#ifdef DeadlockDetection
      auto list = GetEdgeList();
      for (const auto &edge : list) {
        txn_id_t t1 = edge.first;
        txn_id_t t2 = edge.second;
        LOG_INFO("Edge: %d -> %d", t1, t2);
      }
#endif

      txn_id_t target_txn = INVALID_TXN_ID;
      while (HasCycle(&target_txn)) {
#ifdef DeadlockDetection
        LOG_INFO("Cycle Detected %d", target_txn);
#endif
        auto txn = TransactionManager::GetTransaction(target_txn);
        txn->SetState(TransactionState::ABORTED);
        {
          std::lock_guard<std::mutex> table_guard(table_lock_map_latch_);
          for (const auto &table_lock : table_lock_map_) {
            table_lock.second->cv_.notify_all();
          }
        }
        {
          std::lock_guard<std::mutex> row_guard(row_lock_map_latch_);
          for (const auto &row_lock : row_lock_map_) {
            row_lock.second->cv_.notify_all();
          }
        }
        waits_for_.clear();
        BuildGraph();
      }
    }
#ifdef DeadlockDetection
    LOG_INFO("End CycleDetection");
#endif
  }
}
// helper function
template <typename T>
void LockManager::GetEdgeFromLockMap(T &lock_map, std::mutex &lock_map_latch) {
  std::lock_guard<std::mutex> lock_map_guard(lock_map_latch);
  for (auto &element : lock_map) {
    auto &lock_request_queue = element.second->request_queue_;
    std::lock_guard<std::mutex> request_queue_guard(element.second->latch_);

    for (auto &request : lock_request_queue) {
      auto *txn = TransactionManager::GetTransaction(request->txn_id_);

      if (!request->granted_ && txn->GetState() != TransactionState::ABORTED) {
        for (auto &sub_req : lock_request_queue) {
          if (sub_req->granted_) {
            AddEdge(request->txn_id_, sub_req->txn_id_);
          }
        }
      }
    }
  }
}

auto LockManager::LockTableIllegalBehavior(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) const -> bool {
  auto txn_state = txn->GetState();
  if (txn_state != TransactionState::GROWING && txn_state != TransactionState::SHRINKING) {
    return true;
  }
  auto isolation_level = txn->GetIsolationLevel();
  bool is_repeatable_read_shrinking =
      isolation_level == IsolationLevel::REPEATABLE_READ && txn_state == TransactionState::SHRINKING;
  bool is_read_committed_shrinking =
      isolation_level == IsolationLevel::READ_COMMITTED && txn_state == TransactionState::SHRINKING;
  bool is_read_uncommitted_shrinking =
      isolation_level == IsolationLevel::READ_UNCOMMITTED && txn_state == TransactionState::SHRINKING;
  bool is_x_ix_six = lock_mode == LockMode::EXCLUSIVE || lock_mode == LockMode::INTENTION_EXCLUSIVE ||
                     lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;
  bool is_s_is_six = lock_mode == LockMode::SHARED || lock_mode == LockMode::INTENTION_SHARED ||
                     lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;

  if (is_repeatable_read_shrinking || (is_read_committed_shrinking && is_x_ix_six) ||
      (is_read_uncommitted_shrinking && is_x_ix_six)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  }
  if (isolation_level == IsolationLevel::READ_UNCOMMITTED && is_s_is_six) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_SHARED_ON_READ_UNCOMMITTED);
  }

  bool txn_has_lock = false;
  LockMode old_mode = LockMode::EXCLUSIVE;

  if (txn->IsTableSharedLocked(oid)) {
    txn_has_lock = true;
    old_mode = LockMode::SHARED;
  } else if (txn->IsTableExclusiveLocked(oid)) {
    txn_has_lock = true;
    old_mode = LockMode::EXCLUSIVE;
  } else if (txn->IsTableIntentionSharedLocked(oid)) {
    txn_has_lock = true;
    old_mode = LockMode::INTENTION_SHARED;
  } else if (txn->IsTableIntentionExclusiveLocked(oid)) {
    txn_has_lock = true;
    old_mode = LockMode::INTENTION_EXCLUSIVE;
  } else if (txn->IsTableSharedIntentionExclusiveLocked(oid)) {
    txn_has_lock = true;
    old_mode = LockMode::SHARED_INTENTION_EXCLUSIVE;
  }

  if (txn_has_lock) {
    if (old_mode == lock_mode) {
      return false;
    }
    bool is_legal_upgrade =
        (old_mode == LockMode::SHARED &&
         (lock_mode == LockMode::EXCLUSIVE || lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE)) ||
        (old_mode == LockMode::SHARED_INTENTION_EXCLUSIVE && lock_mode == LockMode::EXCLUSIVE) ||
        (old_mode == LockMode::INTENTION_EXCLUSIVE &&
         (lock_mode == LockMode::EXCLUSIVE || lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE)) ||
        (old_mode == LockMode::INTENTION_SHARED &&
         (lock_mode == LockMode::SHARED || lock_mode == LockMode::EXCLUSIVE ||
          lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE || lock_mode == LockMode::INTENTION_EXCLUSIVE));
    if (!is_legal_upgrade) {
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::INCOMPATIBLE_UPGRADE);
    }
  }

  return false;
}
auto LockManager::LockRowIllegalBehavior(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,
                                         const RID &rid) const -> bool {
  auto txn_state = txn->GetState();
  if (txn_state != TransactionState::GROWING && txn_state != TransactionState::SHRINKING) {
    return true;
  }
  auto isolation_level = txn->GetIsolationLevel();
  bool is_repeatable_read_shrinking =
      isolation_level == IsolationLevel::REPEATABLE_READ && txn_state == TransactionState::SHRINKING;
  bool is_read_committed_shrinking_x = isolation_level == IsolationLevel::READ_COMMITTED &&
                                       txn_state == TransactionState::SHRINKING && lock_mode == LockMode::EXCLUSIVE;
  bool is_read_uncommitted_shrinking_x = isolation_level == IsolationLevel::READ_UNCOMMITTED &&
                                         txn_state == TransactionState::SHRINKING && lock_mode == LockMode::EXCLUSIVE;
  bool is_i = lock_mode == LockMode::INTENTION_SHARED || lock_mode == LockMode::INTENTION_EXCLUSIVE ||
              lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;
  bool is_read_uncommitted_s = isolation_level == IsolationLevel::READ_UNCOMMITTED && lock_mode == LockMode::SHARED;

  if (is_i) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_INTENTION_LOCK_ON_ROW);
  }
  if (is_repeatable_read_shrinking || is_read_committed_shrinking_x || is_read_uncommitted_shrinking_x) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  }
  if (is_read_uncommitted_s) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_SHARED_ON_READ_UNCOMMITTED);
  }

  bool txn_has_row_lock = false;
  bool txn_has_table_lock = false;
  LockMode old_row_mode = LockMode::EXCLUSIVE;
  LockMode table_mode = LockMode::EXCLUSIVE;

  if (txn->IsTableSharedLocked(oid)) {
    txn_has_table_lock = true;
    table_mode = LockMode::SHARED;
  } else if (txn->IsTableExclusiveLocked(oid)) {
    txn_has_table_lock = true;
    table_mode = LockMode::EXCLUSIVE;
  } else if (txn->IsTableIntentionSharedLocked(oid)) {
    txn_has_table_lock = true;
    table_mode = LockMode::INTENTION_SHARED;
  } else if (txn->IsTableIntentionExclusiveLocked(oid)) {
    txn_has_table_lock = true;
    table_mode = LockMode::INTENTION_EXCLUSIVE;
  } else if (txn->IsTableSharedIntentionExclusiveLocked(oid)) {
    txn_has_table_lock = true;
    table_mode = LockMode::SHARED_INTENTION_EXCLUSIVE;
  }

  if (txn->IsRowSharedLocked(oid, rid)) {
    txn_has_row_lock = true;
    old_row_mode = LockMode::SHARED;
  } else if (txn->IsRowExclusiveLocked(oid, rid)) {
    txn_has_row_lock = true;
    old_row_mode = LockMode::EXCLUSIVE;
  }

  if (!txn_has_table_lock ||
      (lock_mode == LockMode::EXCLUSIVE && table_mode != LockMode::EXCLUSIVE &&
       table_mode != LockMode::INTENTION_EXCLUSIVE && table_mode != LockMode::SHARED_INTENTION_EXCLUSIVE)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::TABLE_LOCK_NOT_PRESENT);
  }

  if (txn_has_row_lock) {
    if (old_row_mode == lock_mode) {
      return false;
    }
    bool is_legal_upgrade = old_row_mode == LockMode::SHARED && lock_mode == LockMode::EXCLUSIVE;
    if (!is_legal_upgrade) {
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::INCOMPATIBLE_UPGRADE);
    }
  }

  return false;
}

void LockManager::TableCreateNewReqQueue(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) {
  auto new_req_queue = std::make_shared<LockRequestQueue>(new LockRequest(txn->GetTransactionId(), lock_mode, oid));
  new_req_queue->request_queue_.back()->granted_ = true;
  table_lock_map_[oid] = new_req_queue;
}
void LockManager::RowCreateNewReqQueue(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) {
  auto new_req_queue =
      std::make_shared<LockRequestQueue>(new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid));
  new_req_queue->request_queue_.back()->granted_ = true;
  row_lock_map_[rid] = new_req_queue;
}
void LockManager::TableTxnLockSetAddRecord(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) const {
  txn->LockTxn();
  switch (lock_mode) {
    case LockMode::SHARED:
      txn->GetSharedTableLockSet()->insert(oid);
      break;
    case LockMode::EXCLUSIVE:
      txn->GetExclusiveTableLockSet()->insert(oid);
      break;
    case LockMode::INTENTION_SHARED:
      txn->GetIntentionSharedTableLockSet()->insert(oid);
      break;
    case LockMode::INTENTION_EXCLUSIVE:
      txn->GetIntentionExclusiveTableLockSet()->insert(oid);
      break;
    case LockMode::SHARED_INTENTION_EXCLUSIVE:
      txn->GetSharedIntentionExclusiveTableLockSet()->insert(oid);
      break;
    default:
      break;
  }
  txn->UnlockTxn();
}
void LockManager::RowTxnLockSetAddRecord(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,
                                         const RID &rid) const {
  txn->LockTxn();
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();

  switch (lock_mode) {
    case LockMode::SHARED:
      if (s_row_lock_set->find(oid) == txn->GetSharedRowLockSet()->end()) {
        s_row_lock_set->insert({oid, std::unordered_set<RID>{rid}});
      } else {
        s_row_lock_set->find(oid)->second.insert(rid);
      }
      break;
    case LockMode::EXCLUSIVE:
      if (x_row_lock_set->find(oid) == txn->GetExclusiveRowLockSet()->end()) {
        x_row_lock_set->insert({oid, std::unordered_set<RID>{rid}});
      } else {
        x_row_lock_set->find(oid)->second.insert(rid);
      }
      break;
    default:
      break;
  }
  txn->UnlockTxn();
}
void LockManager::TableTxnLockSetDeleteRecord(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) const {
  txn->LockTxn();
  switch (lock_mode) {
    case LockMode::SHARED:
      txn->GetSharedTableLockSet()->erase(oid);
      break;
    case LockMode::EXCLUSIVE:
      txn->GetExclusiveTableLockSet()->erase(oid);
      break;
    case LockMode::INTENTION_SHARED:
      txn->GetIntentionSharedTableLockSet()->erase(oid);
      break;
    case LockMode::INTENTION_EXCLUSIVE:
      txn->GetIntentionExclusiveTableLockSet()->erase(oid);
      break;
    case LockMode::SHARED_INTENTION_EXCLUSIVE:
      txn->GetSharedIntentionExclusiveTableLockSet()->erase(oid);
      break;
    default:
      break;
  }
  txn->UnlockTxn();
}
void LockManager::RowTxnLockSetDeleteRecord(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,
                                            const RID &rid) const {
  txn->LockTxn();
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();
  switch (lock_mode) {
    case LockMode::SHARED:
      s_row_lock_set->find(oid)->second.erase(rid);
      if (s_row_lock_set->find(oid)->second.empty()) {
        s_row_lock_set->erase(oid);
      }
      break;
    case LockMode::EXCLUSIVE:
      x_row_lock_set->find(oid)->second.erase(rid);
      if (x_row_lock_set->find(oid)->second.empty()) {
        x_row_lock_set->erase(oid);
      }
      break;
    default:
      break;
  }
  txn->UnlockTxn();
}
auto LockManager::TryTableLockUpgrade(Transaction *txn, const std::shared_ptr<LockRequestQueue> &req_queue,
                                      LockMode new_mode) const -> bool {
  auto req = GetRequest(req_queue, txn->GetTransactionId());
  if (req != nullptr && req->granted_) {
    LockMode old_mode = req->lock_mode_;
    if (old_mode == new_mode) {
      return true;
    }
    if (req_queue->upgrading_ != INVALID_TXN_ID) {
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
    }
    TableTxnLockSetDeleteRecord(txn, old_mode, req->oid_);
    req_queue->request_queue_.remove(req);
    auto *new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_);
    delete req;
    auto pos = req_queue->request_queue_.begin();
    for (; pos != req_queue->request_queue_.end(); ++pos) {
      bool is_waiting = !(*pos)->granted_;
      if (is_waiting) {
        break;
      }
    }
    req_queue->request_queue_.insert(pos, new_req);
    req_queue->upgrading_ = txn->GetTransactionId();
    bool is_uncompatible = true;
    std::unique_lock<std::mutex> lck(req_queue->latch_, std::adopt_lock);
    while (is_uncompatible) {
      for (auto *temp_req : req_queue->request_queue_) {
        if (temp_req->granted_ && !IsLockCompatible(temp_req->lock_mode_, new_mode)) {
          is_uncompatible = true;
          req_queue->cv_.wait(lck);
          if (txn->GetState() == TransactionState::ABORTED) {
            req_queue->upgrading_ = INVALID_TXN_ID;
            req_queue->request_queue_.remove(new_req);
            delete new_req;
            req_queue->cv_.notify_all();
            return true;
          }
          break;
        }
        is_uncompatible = false;
      }
    }

    new_req->granted_ = true;
    req_queue->upgrading_ = INVALID_TXN_ID;
#ifdef Debug
    LOG_INFO("TableLockUpgrade: txn_id: %d, oid: %d, old_mode: %d, new_mode: %d", txn->GetTransactionId(),
             new_req->oid_, static_cast<int>(old_mode), static_cast<int>(new_mode));
#endif
    return true;
  }
  return false;
}
auto LockManager::GrantLock(const std::shared_ptr<LockRequestQueue> &req_queue, LockRequest *new_req) const -> bool {
  for (auto it = req_queue->request_queue_.begin(); (*it) != new_req; ++it) {
    auto next_it = std::next(it);
    bool cur_is_not_first_watting = *next_it == new_req && !(*it)->granted_;
    bool is_not_compat = (*it)->granted_ && !IsLockCompatible((*it)->lock_mode_, new_req->lock_mode_);

    if (cur_is_not_first_watting || is_not_compat) {
      return false;
    }
  }
  return true;
}

auto LockManager::TryRowLockUpgrade(Transaction *txn, const std::shared_ptr<LockRequestQueue> &req_queue,
                                    LockMode new_mode) const -> bool {
  auto req = GetRequest(req_queue, txn->GetTransactionId());
  if (req != nullptr && req->granted_) {
    LockMode old_mode = req->lock_mode_;
    if (old_mode == new_mode) {
      return true;
    }

    if (req_queue->upgrading_ != INVALID_TXN_ID) {
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
    }
    RowTxnLockSetDeleteRecord(txn, old_mode, req->oid_, req->rid_);
    req_queue->request_queue_.remove(req);
    auto *new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_, req->rid_);
    delete req;

    auto pos = req_queue->request_queue_.begin();
    for (; pos != req_queue->request_queue_.end(); ++pos) {
      bool is_waiting = !(*pos)->granted_;
      if (is_waiting) {
        break;
      }
    }
    req_queue->request_queue_.insert(pos, new_req);
    req_queue->upgrading_ = txn->GetTransactionId();

    bool is_uncompatible = true;
    std::unique_lock<std::mutex> lck(req_queue->latch_, std::adopt_lock);
    while (is_uncompatible) {
      for (auto *temp_req : req_queue->request_queue_) {
        if (temp_req->granted_ && !IsLockCompatible(temp_req->lock_mode_, new_mode)) {
          is_uncompatible = true;
          req_queue->cv_.wait(lck);
          if (txn->GetState() == TransactionState::ABORTED) {
            req_queue->upgrading_ = INVALID_TXN_ID;
            req_queue->request_queue_.remove(new_req);
            delete new_req;
            req_queue->cv_.notify_all();
            return true;
          }
          break;
        }
        is_uncompatible = false;
      }
    }

    new_req->granted_ = true;
    req_queue->upgrading_ = INVALID_TXN_ID;
#ifdef Debug
    LOG_INFO("RowLockUpgrade: txn_id: %d, oid: %d, old_mode: %d, new_mode: %d", txn->GetTransactionId(), new_req->oid_,
             static_cast<int>(old_mode), static_cast<int>(new_mode));
#endif
    return true;
  }
  return false;
}

auto LockManager::IsLockCompatible(LockMode old_mode, LockMode new_mode) const -> bool {
  switch (old_mode) {
    case LockMode::SHARED:
      return new_mode == LockMode::SHARED || new_mode == LockMode::INTENTION_SHARED;
    case LockMode::INTENTION_SHARED:
      return new_mode != LockMode::EXCLUSIVE;
    case LockMode::INTENTION_EXCLUSIVE:
      return new_mode == LockMode::INTENTION_SHARED || new_mode == LockMode::INTENTION_EXCLUSIVE;
    case LockMode::SHARED_INTENTION_EXCLUSIVE:
      return new_mode == LockMode::INTENTION_SHARED;
    default:
      return false;
  }
}

auto LockManager::UnlockTableIllegalBehavior(Transaction *txn, const table_oid_t &oid) const -> bool {
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();

  if ((s_row_lock_set->count(oid) > 0 && !s_row_lock_set->at(oid).empty()) ||
      (x_row_lock_set->count(oid) > 0 && !x_row_lock_set->at(oid).empty())) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::TABLE_UNLOCKED_BEFORE_UNLOCKING_ROWS);
    return true;
  }
  if (!txn->IsTableExclusiveLocked(oid) && !txn->IsTableIntentionExclusiveLocked(oid) &&
      !txn->IsTableIntentionSharedLocked(oid) && !txn->IsTableSharedIntentionExclusiveLocked(oid) &&
      !txn->IsTableSharedLocked(oid)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}
auto LockManager::UnlockRowIllegalBehavior(Transaction *txn, const table_oid_t &oid, const RID &rid) const -> bool {
  if (!txn->IsRowExclusiveLocked(oid, rid) && !txn->IsRowSharedLocked(oid, rid)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}

void LockManager::UpdateTxnState(Transaction *txn, LockMode lock_mode) const {
  if (txn->GetState() == TransactionState::GROWING) {
    auto isolation_level = txn->GetIsolationLevel();
    if (lock_mode == LockMode::EXCLUSIVE ||
        (isolation_level == IsolationLevel::REPEATABLE_READ && lock_mode == LockMode::SHARED)) {
      txn->SetState(TransactionState::SHRINKING);
    }
  }
}

auto LockManager::GetRequest(const std::shared_ptr<LockRequestQueue> &req_queue, txn_id_t txn_id) const
    -> LockRequest * {
  for (auto &it : req_queue->request_queue_) {
    if (it->txn_id_ == txn_id) {
      return it;
    }
  }
  return nullptr;
}

auto LockManager::DFS(txn_id_t start, std::unordered_map<txn_id_t, bool> &visited,
                      std::stack<txn_id_t> &recursion_stack, txn_id_t &min_id) -> bool {
  bool result = false;
  std::vector<txn_id_t> &neighbors = waits_for_[start];
  for (const txn_id_t &neighbor : neighbors) {
    auto it = visited.find(neighbor);
    if (it == visited.end()) {
      recursion_stack.push(neighbor);
      visited.emplace(neighbor, true);
      if (DFS(neighbor, visited, recursion_stack, min_id)) {
        result = true;
        break;
      }
    }
    if (it != visited.end() && it->second) {
      min_id = GetMinTxnID(neighbor, recursion_stack);
      result = true;
      break;
    }
  }
  visited.insert_or_assign(recursion_stack.top(), false);
  recursion_stack.pop();
  return result;
}
auto LockManager::GetMinTxnID(txn_id_t vertex, std::stack<txn_id_t> &recursion_stack) -> txn_id_t {
  txn_id_t min_id = INVALID_TXN_ID;
  std::stack<txn_id_t> stack;
  stack.push(recursion_stack.top());
  recursion_stack.pop();

  while (stack.top() != vertex) {
    stack.push(recursion_stack.top());
    recursion_stack.pop();
  }
  while (!stack.empty()) {
    min_id = std::max(min_id, stack.top());
    recursion_stack.push(stack.top());
    stack.pop();
  }

  return min_id;
}

void LockManager::BuildGraph() {
  {
    std::lock_guard guard{table_lock_map_latch_};
    for (const auto &itr : table_lock_map_) {
      std::lock_guard guard{itr.second->latch_};
      AddEdges(itr.second->request_queue_);
    }
  }

  {
    std::lock_guard guard{row_lock_map_latch_};
    for (const auto &itr : row_lock_map_) {
      std::lock_guard guard{itr.second->latch_};
      AddEdges(itr.second->request_queue_);
    }
  }
}
void LockManager::AddEdges(const std::list<LockRequest *> &lck_reqs) {
  std::vector<txn_id_t> granteds;
  std::vector<txn_id_t> waitings;
  for (const auto lck_req : lck_reqs) {
    const auto txn = TransactionManager::GetTransaction(lck_req->txn_id_);
    if (txn->GetState() == TransactionState::ABORTED) {
      continue;
    }

    if (lck_req->granted_) {
      granteds.push_back(lck_req->txn_id_);
    } else {
      waitings.push_back(lck_req->txn_id_);
    }
  }

  for (auto &&t1 : waitings) {
    for (auto &&t2 : granteds) {
      AddEdge(t1, t2);
    }
  }
}

// end of helper function
}  // namespace bustub
