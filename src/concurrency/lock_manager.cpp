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
// helper function
auto LockManager::LockTableIllegalBehavior(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) const -> bool {
  auto txn_state = txn->GetState();
  if (txn_state != TransactionState::GROWING && txn_state != TransactionState::SHRINKING) {
    return true;
  }
  auto isolation_level = txn->GetIsolationLevel();
  bool is_repeatable_read_shrinking = isolation_level == IsolationLevel::REPEATABLE_READ && txn_state == TransactionState::SHRINKING;
  bool is_read_committed_shrinking = isolation_level == IsolationLevel::READ_COMMITTED && txn_state == TransactionState::SHRINKING;
  bool is_read_uncommitted_shrinking = isolation_level == IsolationLevel::READ_UNCOMMITTED && txn_state == TransactionState::SHRINKING;
  bool is_x_ix_six = lock_mode == LockMode::EXCLUSIVE || lock_mode == LockMode::INTENTION_EXCLUSIVE || lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;
  bool is_s_is_six = lock_mode == LockMode::SHARED || lock_mode == LockMode::INTENTION_SHARED || lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;

  if(is_repeatable_read_shrinking || (is_read_committed_shrinking&&is_x_ix_six) || (is_read_uncommitted_shrinking&&is_x_ix_six)){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  } else if (isolation_level == IsolationLevel::READ_UNCOMMITTED && is_s_is_six) {
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
  bool is_repeatable_read_shrinking = isolation_level == IsolationLevel::REPEATABLE_READ && txn_state == TransactionState::SHRINKING;
  bool is_read_committed_shrinking_x = isolation_level == IsolationLevel::READ_COMMITTED && txn_state == TransactionState::SHRINKING && lock_mode == LockMode::EXCLUSIVE;
  bool is_read_uncommitted_shrinking_x = isolation_level == IsolationLevel::READ_UNCOMMITTED && txn_state == TransactionState::SHRINKING && lock_mode == LockMode::EXCLUSIVE;
  bool is_i = lock_mode == LockMode::INTENTION_SHARED || lock_mode == LockMode::INTENTION_EXCLUSIVE || lock_mode == LockMode::SHARED_INTENTION_EXCLUSIVE;
  bool is_read_uncommitted_s = isolation_level == IsolationLevel::READ_UNCOMMITTED && lock_mode == LockMode::SHARED;

  if (is_i) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_INTENTION_LOCK_ON_ROW);
  } else if (is_repeatable_read_shrinking || is_read_committed_shrinking_x || is_read_uncommitted_shrinking_x) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  } else if (is_read_uncommitted_s) {
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

  if(txn->IsRowSharedLocked(oid, rid)){
    txn_has_row_lock = true;
    old_row_mode = LockMode::SHARED;
  } else if(txn->IsRowExclusiveLocked(oid, rid)){
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
  TableTxnLockSetAddRecord(txn, lock_mode, oid);
}
void LockManager::RowCreateNewReqQueue(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) {
  auto new_req_queue =
      std::make_shared<LockRequestQueue>(new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid));
  new_req_queue->request_queue_.back()->granted_ = true;
  row_lock_map_[rid] = new_req_queue;
  RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
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
      if(s_row_lock_set->find(oid)->second.empty()){
        s_row_lock_set->erase(oid);
      }
      break;
    case LockMode::EXCLUSIVE:
      x_row_lock_set->find(oid)->second.erase(rid);
      if(x_row_lock_set->find(oid)->second.empty()){
        x_row_lock_set->erase(oid);
      }
      break;
    default:
      break;
  }
  txn->UnlockTxn();
}
auto LockManager::TryTableLockUpgrade(Transaction *txn, std::shared_ptr<LockRequestQueue> req_queue,
                                      LockMode new_mode) const -> bool {
  for (auto *req : req_queue->request_queue_) {
    if (req->txn_id_ == txn->GetTransactionId() && req->granted_) {
      LockMode old_mode = req->lock_mode_;
      if (old_mode == new_mode) {
        return true;
      }
      auto *new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_);
      req_queue->request_queue_.emplace_back(new_req);
      if (IsConcurrentLockUpgrades(req_queue, txn->GetTransactionId())) {
        req_queue->request_queue_.remove(new_req);
        txn->SetState(TransactionState::ABORTED);
        throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
      }
      req_queue->request_queue_.remove(new_req);
      delete new_req;
      req->lock_mode_ = new_mode;
      #ifdef Debug
      LOG_INFO("TableLockUpgrade: txn_id: %d, oid: %d, old_mode: %d, new_mode: %d", txn->GetTransactionId(), req->oid_, int(old_mode), int(new_mode));
      #endif
      TableTxnLockSetDeleteRecord(txn, old_mode, req->oid_);
      TableTxnLockSetAddRecord(txn, new_mode, req->oid_);
      return true;
    }
  }
  return false;
}

auto LockManager::TryRowLockUpgrade(Transaction *txn, std::shared_ptr<LockRequestQueue> req_queue,
                                    LockMode new_mode) const -> bool {
  for (auto *req : req_queue->request_queue_) {
    if (req->txn_id_ == txn->GetTransactionId() && req->granted_) {
      LockMode old_mode = req->lock_mode_;
      if (old_mode == new_mode) {
        return true;
      }
      auto *new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_, req->rid_);
      req_queue->request_queue_.emplace_back(new_req);
      if (IsConcurrentLockUpgrades(req_queue, txn->GetTransactionId())) {
        req_queue->request_queue_.remove(new_req);
        txn->SetState(TransactionState::ABORTED);
        throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
      }
      req_queue->request_queue_.remove(new_req);
      delete new_req;
      req->lock_mode_ = new_mode;
      RowTxnLockSetDeleteRecord(txn, old_mode, req->oid_, req->rid_);
      RowTxnLockSetAddRecord(txn, new_mode, req->oid_, req->rid_);
      return true;
    }
  }
  return false;
}

auto LockManager::IsConcurrentLockUpgrades(std::shared_ptr<LockRequestQueue> req_queue, txn_id_t txn_id) const -> bool {
  std::unordered_set<txn_id_t> wait_lock_upgrade_set;
  for (auto rit = req_queue->request_queue_.rbegin(); rit != req_queue->request_queue_.rend(); ++rit) {
    if (!(*rit)->granted_ && (*rit)->txn_id_ != txn_id) {
      wait_lock_upgrade_set.insert((*rit)->txn_id_);
    } else {
      if (wait_lock_upgrade_set.find((*rit)->txn_id_) != wait_lock_upgrade_set.end()) {
#ifdef Debug
        LOG_INFO("Concurrent Lock Upgrades: txn_id: %d", (*rit)->txn_id_);
#endif
        return true;
      }
    }
  }
  return false;
}

auto LockManager::TableIsLockCompatible(std::shared_ptr<LockRequestQueue> req_queue, LockMode new_mode) const -> bool {
  if (new_mode == LockMode::EXCLUSIVE) {
    return false;
  }
  for (auto rit = req_queue->request_queue_.rbegin(); rit != req_queue->request_queue_.rend(); ++rit) {
    if (!(*rit)->granted_) {
      return false;
    }
    auto old_mode = (*rit)->lock_mode_;
    if (old_mode == LockMode::INTENTION_SHARED) {
      return true;
    } else if (old_mode == LockMode::INTENTION_EXCLUSIVE &&
               (new_mode == LockMode::INTENTION_SHARED || new_mode == LockMode::INTENTION_EXCLUSIVE)) {
      return true;
    } else if (old_mode == LockMode::SHARED &&
               (new_mode == LockMode::SHARED || new_mode == LockMode::INTENTION_SHARED)) {
      return true;
    } else if (old_mode == LockMode::SHARED_INTENTION_EXCLUSIVE && new_mode == LockMode::INTENTION_SHARED) {
      return true;
    }
    break;
  }
  return false;
}
auto LockManager::RowIsLockCompatible(std::shared_ptr<LockRequestQueue> req_queue, LockMode new_mode) const -> bool {
  if (new_mode == LockMode::EXCLUSIVE) {
    return false;
  }
  for (auto rit = req_queue->request_queue_.rbegin(); rit != req_queue->request_queue_.rend(); ++rit) {
    if (!(*rit)->granted_) {
      return false;
    }
    auto old_mode = (*rit)->lock_mode_;
    if (old_mode == LockMode::SHARED && new_mode == LockMode::SHARED) {
      return true;
    }
    break;
  }
  return false;
}
void LockManager::TableWaitLock(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,
                                std::shared_ptr<LockRequestQueue> req_queue) const {
  auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
  req_queue->request_queue_.emplace_back(new_req);
  req_queue->latch_.unlock();
  std::unique_lock<std::mutex> lck(req_queue->latch_);
  req_queue->cv_.wait(lck,[&]{ return req_queue->request_queue_.front()==new_req; });
  new_req->granted_ = true;
  TableTxnLockSetAddRecord(txn, lock_mode, oid);
}
void LockManager::RowWaitLock(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,
                              std::shared_ptr<LockRequestQueue> req_queue, const RID &rid) const {
  auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid);
  req_queue->request_queue_.emplace_back(new_req);
  req_queue->latch_.unlock();
  std::unique_lock<std::mutex> lck(req_queue->latch_);
  req_queue->cv_.wait(lck,[&]{ return req_queue->request_queue_.front()==new_req; });
  new_req->granted_ = true;
  RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
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
  if (!txn->IsTableExclusiveLocked(oid) &&
      !txn->IsTableIntentionExclusiveLocked(oid) &&
      !txn->IsTableIntentionSharedLocked(oid) &&
      !txn->IsTableSharedIntentionExclusiveLocked(oid) &&
      !txn->IsTableSharedLocked(oid)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}
auto LockManager::UnlockRowIllegalBehavior(Transaction *txn, const table_oid_t &oid, const RID &rid) const -> bool {
  if (!txn->IsRowExclusiveLocked(oid,rid) && !txn->IsRowSharedLocked(oid,rid)) {
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}

void LockManager::UpdateTxnState(Transaction *txn, LockMode lock_mode) const {
  if (txn->GetState()==TransactionState::GROWING) {
    auto isolation_level = txn->GetIsolationLevel();
    if(lock_mode==LockMode::EXCLUSIVE||
    (isolation_level==IsolationLevel::REPEATABLE_READ && lock_mode==LockMode::SHARED)){
      txn->SetState(TransactionState::SHRINKING);
    }
  }
}
// end of helper function
auto LockManager::LockTable(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("LockTable: txn_id: %d, lock_mode: %d, oid: %d,iso level: %d", txn->GetTransactionId(), int(lock_mode), oid,
           int(txn->GetIsolationLevel()));
#endif
  if (LockTableIllegalBehavior(txn, lock_mode, oid)) {
    #ifdef Debug
    LOG_INFO("LockTableIllegalBehavior: txn_id: %d", txn->GetTransactionId());
    #endif
    return false;
  }
  table_lock_map_latch_.lock();
  auto req_queue_itr = table_lock_map_.find(oid);
  bool is_queue_exist = req_queue_itr!=table_lock_map_.end();
  if (!is_queue_exist) {
    TableCreateNewReqQueue(txn, lock_mode, oid);
    table_lock_map_latch_.unlock();
    return true;
  }
  table_lock_map_latch_.unlock();
  auto &que_latch = req_queue_itr->second->latch_;
  que_latch.lock();

  bool is_queue_empty = req_queue_itr->second->request_queue_.empty();
  if(is_queue_empty){
    auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    que_latch.unlock();
    TableTxnLockSetAddRecord(txn, lock_mode, oid);
    return true;
  }
  
  if (TryTableLockUpgrade(txn, req_queue_itr->second, lock_mode)) {
    que_latch.unlock();
    return true;
  }

  if (TableIsLockCompatible(req_queue_itr->second, lock_mode)) {
    // give the lock
    auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    que_latch.unlock();
    TableTxnLockSetAddRecord(txn, lock_mode, oid);
  } else {
#ifdef Debug
    LOG_INFO("Wait Table Lock: txn_id: %d,oid: %d", txn->GetTransactionId(),oid);
#endif
    TableWaitLock(txn, lock_mode, oid, req_queue_itr->second);
#ifdef Debug
    LOG_INFO("Wait Table Lock Done: txn_id: %d", txn->GetTransactionId());
#endif
  }
  return true;
}

auto LockManager::UnlockTable(Transaction *txn, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("UnLockTable: txn_id: %d,oid: %d,iso level: %d", txn->GetTransactionId(), oid,
           int(txn->GetIsolationLevel()));
#endif
  if (UnlockTableIllegalBehavior(txn, oid)) {
    #ifdef Debug
    LOG_INFO("UnlockTableIllegalBehavior: txn_id: %d", txn->GetTransactionId());
    #endif
    return false;
  }
  table_lock_map_latch_.lock();
  // update lock table
  LockRequest *txn_req = nullptr;
  auto req_queue_itr = table_lock_map_.find(oid);
  auto &que_latch = req_queue_itr->second->latch_;
  que_latch.lock();
  auto req_queue_ptr = req_queue_itr->second;
  for (auto *req : req_queue_ptr->request_queue_) {
    if (req->txn_id_ == txn->GetTransactionId()) {
      txn_req = req;
      break;
    }
  }
  auto lock_mode = txn_req->lock_mode_;
  UpdateTxnState(txn, lock_mode);
  TableTxnLockSetDeleteRecord(txn, lock_mode, oid);

  auto *last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = last_req->granted_ == false;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  table_lock_map_latch_.unlock();

  if (has_wait_txn) {
    req_queue_itr->second->cv_.notify_all();
  }
  que_latch.unlock();
  return true;
}

auto LockManager::LockRow(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) -> bool {
#ifdef Debug
  LOG_INFO("LockRow: txn_id: %d, lock_mode: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(), int(lock_mode),
           oid, rid.ToString().c_str(), int(txn->GetIsolationLevel()));
#endif
  if (LockRowIllegalBehavior(txn, lock_mode, oid, rid)) {
    #ifdef Debug
    LOG_INFO("LockRowIllegalBehavior: txn_id: %d", txn->GetTransactionId());
    #endif
    return false;
  }
  row_lock_map_latch_.lock();
  auto req_queue_itr = row_lock_map_.find(rid);
  bool is_queue_exist = req_queue_itr!=row_lock_map_.end();
  if (!is_queue_exist) {
    RowCreateNewReqQueue(txn, lock_mode, oid, rid);
    row_lock_map_latch_.unlock();
    return true;
  }
  row_lock_map_latch_.unlock();
  auto &que_latch = req_queue_itr->second->latch_;
  que_latch.lock();
  bool is_queue_empty = req_queue_itr->second->request_queue_.empty();
  if(is_queue_empty){
    auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid,rid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    que_latch.unlock();
    RowTxnLockSetAddRecord(txn, lock_mode, oid,rid);
    return true;
  }
  if (TryTableLockUpgrade(txn, req_queue_itr->second, lock_mode)) {
    que_latch.unlock();
    return true;
  }

  if (RowIsLockCompatible(req_queue_itr->second, lock_mode)) {
    // give the lock
    auto *new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid, rid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    que_latch.unlock();
    RowTxnLockSetAddRecord(txn, lock_mode, oid, rid);
    return true;
  } else {
#ifdef Debug
    LOG_INFO("Wait Row Lock: txn_id: %d", txn->GetTransactionId());
#endif
    RowWaitLock(txn, lock_mode, oid, req_queue_itr->second, rid);
#ifdef Debug
    LOG_INFO("Wait Row Lock Done: txn_id: %d", txn->GetTransactionId());
#endif
  }
  return true;
}

auto LockManager::UnlockRow(Transaction *txn, const table_oid_t &oid, const RID &rid) -> bool {
#ifdef Debug
  LOG_INFO("UnLockRow: txn_id: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(), oid, rid.ToString().c_str(),
           int(txn->GetIsolationLevel()));
#endif
  if (UnlockRowIllegalBehavior(txn, oid,rid)) {
    #ifdef Debug
    LOG_INFO("UnlockRowIllegalBehavior: txn_id: %d", txn->GetTransactionId());
    #endif
    return false;
  }
  row_lock_map_latch_.lock();
  // update lock table
  LockRequest *txn_req = nullptr;
  auto req_queue_itr = row_lock_map_.find(rid);
  auto &que_latch = req_queue_itr->second->latch_;
  que_latch.lock();

  auto req_queue_ptr = req_queue_itr->second;
  for (auto *req : req_queue_ptr->request_queue_) {
    if (req->txn_id_ == txn->GetTransactionId()) {
      txn_req = req;
      break;
    }
  }
  auto lock_mode = txn_req->lock_mode_;
#ifdef Debug
  LOG_INFO("lock_mode: %d", int(lock_mode));
#endif
  UpdateTxnState(txn, lock_mode);
  RowTxnLockSetDeleteRecord(txn, lock_mode, oid, rid);

  auto *last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = last_req->granted_ == false;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  if (req_queue_ptr->request_queue_.empty()) {
    row_lock_map_.erase(rid);
  }
  row_lock_map_latch_.unlock();

  if (has_wait_txn) {
    req_queue_itr->second->cv_.notify_all();
  }
  que_latch.unlock();
  return true;
}

void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) {}

void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) {}

auto LockManager::HasCycle(txn_id_t *txn_id) -> bool { return false; }

auto LockManager::GetEdgeList() -> std::vector<std::pair<txn_id_t, txn_id_t>> {
  std::vector<std::pair<txn_id_t, txn_id_t>> edges(0);
  return edges;
}

void LockManager::RunCycleDetection() {
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(cycle_detection_interval);
    {  // TODO(students): detect deadlock
    }
  }
}

}  // namespace bustub
