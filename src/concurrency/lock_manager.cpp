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
auto LockManager::LockTableIllegalBehavior(Transaction *txn, LockMode lock_mode,const table_oid_t &oid) const -> bool {
  bool is_illegal = false;
  auto txn_state = txn->GetState();
  auto isolation_level = txn->GetIsolationLevel();

  if(txn_state == TransactionState::SHRINKING && (lock_mode==LockMode::EXCLUSIVE || lock_mode==LockMode::INTENTION_EXCLUSIVE)){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  } else if(isolation_level== IsolationLevel::READ_UNCOMMITTED && (lock_mode==LockMode::SHARED || lock_mode==LockMode::INTENTION_SHARED || lock_mode==LockMode::SHARED_INTENTION_EXCLUSIVE)){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_SHARED_ON_READ_UNCOMMITTED);
  } 

  bool txn_has_lock = false;
  LockMode old_mode = LockMode::EXCLUSIVE;
  auto s_table_lock_set = txn->GetSharedTableLockSet();
  auto x_table_lock_set = txn->GetExclusiveTableLockSet();
  auto is_table_lock_set = txn->GetIntentionSharedTableLockSet();
  auto ix_table_lock_set = txn->GetIntentionExclusiveTableLockSet();
  auto six_table_lock_set = txn->GetSharedIntentionExclusiveTableLockSet();

  if(s_table_lock_set->find(oid)!=s_table_lock_set->end()){
    txn_has_lock = true;
    old_mode = LockMode::SHARED;
  }else if(x_table_lock_set->find(oid)!=x_table_lock_set->end()){
    txn_has_lock = true;
    old_mode = LockMode::EXCLUSIVE;
  }else if(is_table_lock_set->find(oid)!=is_table_lock_set->end()){
    txn_has_lock = true;
    old_mode = LockMode::INTENTION_SHARED;
  }else if(ix_table_lock_set->find(oid)!=ix_table_lock_set->end()){
    txn_has_lock = true;
    old_mode = LockMode::INTENTION_EXCLUSIVE;
  }else if(six_table_lock_set->find(oid)!=six_table_lock_set->end()){
    txn_has_lock = true;
    old_mode = LockMode::SHARED_INTENTION_EXCLUSIVE;
  }

  if(txn_has_lock){
    if(old_mode==lock_mode){
      return false;
    }
    bool is_legal_upgrade = 
        (old_mode==LockMode::SHARED && (lock_mode==LockMode::EXCLUSIVE || lock_mode==LockMode::SHARED_INTENTION_EXCLUSIVE)) ||
        (old_mode==LockMode::SHARED_INTENTION_EXCLUSIVE && lock_mode==LockMode::EXCLUSIVE) ||
        (old_mode==LockMode::INTENTION_EXCLUSIVE && (lock_mode==LockMode::EXCLUSIVE || lock_mode==LockMode::SHARED_INTENTION_EXCLUSIVE)) ||
        (old_mode==LockMode::INTENTION_SHARED && (lock_mode==LockMode::SHARED || lock_mode==LockMode::EXCLUSIVE || lock_mode==LockMode::SHARED_INTENTION_EXCLUSIVE || lock_mode==LockMode::INTENTION_EXCLUSIVE));
    is_illegal = !is_legal_upgrade;
    if(!is_legal_upgrade){
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::INCOMPATIBLE_UPGRADE);
    }
  }
  if((isolation_level==IsolationLevel::REPEATABLE_READ || isolation_level==IsolationLevel::READ_UNCOMMITTED) && txn_state==TransactionState::SHRINKING){
    is_illegal = true;
  } else if(txn_state!=TransactionState::GROWING && txn_state!=TransactionState::SHRINKING){
    is_illegal = true;
  } else if(isolation_level==IsolationLevel::READ_COMMITTED && txn_state==TransactionState::SHRINKING && lock_mode!=LockMode::SHARED && lock_mode!=LockMode::INTENTION_SHARED){
    is_illegal = true;
  } else if(isolation_level==IsolationLevel::READ_UNCOMMITTED && txn_state==TransactionState::GROWING && lock_mode!=LockMode::EXCLUSIVE && lock_mode!=LockMode::INTENTION_EXCLUSIVE){
    is_illegal = true;
  }
  return is_illegal;
}
auto LockManager::LockRowIllegalBehavior(Transaction *txn, LockMode lock_mode,const table_oid_t &oid, const RID &rid) const -> bool{
  bool is_illegal = false;
  auto txn_state = txn->GetState();
  auto isolation_level = txn->GetIsolationLevel();

  if(lock_mode==LockMode::INTENTION_EXCLUSIVE || lock_mode==LockMode::INTENTION_SHARED || lock_mode==LockMode::SHARED_INTENTION_EXCLUSIVE){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_INTENTION_LOCK_ON_ROW);
  } else if(txn_state == TransactionState::SHRINKING && lock_mode==LockMode::EXCLUSIVE){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
  } else if(isolation_level== IsolationLevel::READ_UNCOMMITTED && lock_mode==LockMode::SHARED){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_SHARED_ON_READ_UNCOMMITTED);
  } 

  bool txn_has_row_lock = false;
  bool txn_has_table_lock = false;
  LockMode old_row_mode = LockMode::EXCLUSIVE;
  LockMode table_mode = LockMode::EXCLUSIVE;
  auto s_table_lock_set = txn->GetSharedTableLockSet();
  auto x_table_lock_set = txn->GetExclusiveTableLockSet();
  auto is_table_lock_set = txn->GetIntentionSharedTableLockSet();
  auto ix_table_lock_set = txn->GetIntentionExclusiveTableLockSet();
  auto six_table_lock_set = txn->GetSharedIntentionExclusiveTableLockSet();
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();

  if(s_table_lock_set->find(oid)!=s_table_lock_set->end()){
    txn_has_table_lock = true;
    table_mode = LockMode::SHARED;
  }else if(x_table_lock_set->find(oid)!=x_table_lock_set->end()){
    txn_has_table_lock = true;
    table_mode = LockMode::EXCLUSIVE;
  }else if(is_table_lock_set->find(oid)!=is_table_lock_set->end()){
    txn_has_table_lock = true;
    table_mode = LockMode::INTENTION_SHARED;
  }else if(ix_table_lock_set->find(oid)!=ix_table_lock_set->end()){
    txn_has_table_lock = true;
    table_mode = LockMode::INTENTION_EXCLUSIVE;
  }else if(six_table_lock_set->find(oid)!=six_table_lock_set->end()){
    txn_has_table_lock = true;
    table_mode = LockMode::SHARED_INTENTION_EXCLUSIVE;
  }

  if(!txn_has_table_lock || 
  (lock_mode==LockMode::EXCLUSIVE && table_mode!=LockMode::EXCLUSIVE && table_mode!=LockMode::INTENTION_EXCLUSIVE && table_mode!=LockMode::SHARED_INTENTION_EXCLUSIVE)){
    is_illegal = true;
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::TABLE_LOCK_NOT_PRESENT);
  }

  if(txn_has_row_lock){
    if(old_row_mode==lock_mode){
      return false;
    }
    bool is_legal_upgrade = old_row_mode==LockMode::SHARED && lock_mode==LockMode::EXCLUSIVE;
    is_illegal = !is_legal_upgrade;
    if(!is_legal_upgrade){
      txn->SetState(TransactionState::ABORTED);
      throw TransactionAbortException(txn->GetTransactionId(), AbortReason::INCOMPATIBLE_UPGRADE);
    }
  }

  if((isolation_level==IsolationLevel::REPEATABLE_READ || isolation_level==IsolationLevel::READ_UNCOMMITTED) && txn_state==TransactionState::SHRINKING){
    is_illegal = true;
  } else if(txn_state!=TransactionState::GROWING && txn_state!=TransactionState::SHRINKING){
    is_illegal = true;
  }
  return is_illegal;
}


void LockManager::TableCreateNewReqQueue(Transaction *txn, LockMode lock_mode, const table_oid_t &oid){
  auto new_req_queue = std::make_shared<LockRequestQueue>(new LockRequest(txn->GetTransactionId(), lock_mode, oid));
  new_req_queue->request_queue_.back()->granted_ = true;
  table_lock_map_[oid] = new_req_queue;
  TableTxnLockSetAddRecord(txn, lock_mode, oid);
}
void LockManager::RowCreateNewReqQueue(Transaction *txn, LockMode lock_mode, const table_oid_t &oid ,const RID &rid){
  auto new_req_queue = std::make_shared<LockRequestQueue>(new LockRequest(txn->GetTransactionId(), lock_mode, oid,rid));
  new_req_queue->request_queue_.back()->granted_ = true;
  row_lock_map_[rid] = new_req_queue;
  RowTxnLockSetAddRecord(txn, lock_mode, oid,rid);
}
void LockManager::TableTxnLockSetAddRecord(Transaction *txn,LockMode lock_mode ,const table_oid_t &oid ) const {
  switch (lock_mode)
  {
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
}
void LockManager::RowTxnLockSetAddRecord(Transaction *txn,LockMode lock_mode ,const table_oid_t &oid, const RID &rid ) const {
  switch (lock_mode)
  {
  case LockMode::SHARED:
    txn->GetSharedRowLockSet()->find(oid)->second.insert(rid);
    break;
  case LockMode::EXCLUSIVE:
    txn->GetExclusiveRowLockSet()->find(oid)->second.insert(rid);
    break;
  default:
    break;
  }
}
void LockManager::TableTxnLockSetDeleteRecord(Transaction *txn,LockMode lock_mode ,const table_oid_t &oid ) const {
  switch (lock_mode)
  {
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
}
void LockManager::RowTxnLockSetDeleteRecord(Transaction *txn,LockMode lock_mode ,const table_oid_t &oid ,const RID &rid ) const{
  switch (lock_mode)
  {
  case LockMode::SHARED:
    txn->GetSharedRowLockSet()->find(oid)->second.erase(rid);
    break;
  case LockMode::EXCLUSIVE:
    txn->GetExclusiveRowLockSet()->find(oid)->second.erase(rid);
    break;
  default:
    break;
  }
}
auto LockManager::TryTableLockUpgrade(Transaction *txn, std::shared_ptr<LockRequestQueue> req_queue, LockMode new_mode) const -> bool{
  for(auto* req:req_queue->request_queue_){
    if(req->txn_id_ == txn->GetTransactionId() && req->granted_){
      LockMode old_mode = req->lock_mode_;
      if(old_mode==new_mode){
        return true;
      }
      auto* new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_);
      req_queue->request_queue_.emplace_back(new_req);
      if(IsConcurrentLockUpgrades(req_queue,txn->GetTransactionId())){
        req_queue->request_queue_.remove(new_req);
        txn->SetState(TransactionState::ABORTED);
        throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
      }
      req_queue->request_queue_.remove(new_req);
      req->lock_mode_ = new_mode;
      TableTxnLockSetDeleteRecord(txn, old_mode, req->oid_);
      TableTxnLockSetAddRecord(txn, new_mode, req->oid_);
      return true;
    }
  }
  return false;
}

auto LockManager::TryRowLockUpgrade(Transaction *txn, std::shared_ptr<LockRequestQueue> req_queue, LockMode new_mode) const -> bool{
  for(auto* req:req_queue->request_queue_){
    if(req->txn_id_ == txn->GetTransactionId() && req->granted_){
      LockMode old_mode = req->lock_mode_;
      if(old_mode==new_mode){
        return true;
      }
      auto* new_req = new LockRequest(txn->GetTransactionId(), new_mode, req->oid_,req->rid_);
      req_queue->request_queue_.emplace_back(new_req);
      if(IsConcurrentLockUpgrades(req_queue,txn->GetTransactionId())){
        req_queue->request_queue_.remove(new_req);
        txn->SetState(TransactionState::ABORTED);
        throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
      }
      req_queue->request_queue_.remove(new_req);
      req->lock_mode_ = new_mode;
      RowTxnLockSetDeleteRecord(txn, old_mode, req->oid_,req->rid_);
      RowTxnLockSetAddRecord(txn, new_mode, req->oid_,req->rid_);
      return true;
    }
  }
  return false;
}

auto LockManager::IsConcurrentLockUpgrades(std::shared_ptr<LockRequestQueue> req_queue,txn_id_t txn_id) const -> bool{
  std::unordered_set<txn_id_t> wait_lock_upgrade_set;
  for(auto rit = req_queue->request_queue_.rbegin(); rit!=req_queue->request_queue_.rend(); ++rit){
    if(!(*rit)->granted_ && (*rit)->txn_id_!=txn_id){
      wait_lock_upgrade_set.insert((*rit)->txn_id_);
    } else {
      if(wait_lock_upgrade_set.find((*rit)->txn_id_)!=wait_lock_upgrade_set.end()){
        return true;
      }
    }
  }
  return false;
}

auto LockManager::TableIsLockCompatible(std::shared_ptr<LockRequestQueue> req_queue,LockMode new_mode)const ->bool{
  if(new_mode==LockMode::EXCLUSIVE){
    return false;
  }
  for(auto rit = req_queue->request_queue_.rbegin();rit!=req_queue->request_queue_.rend();++rit){
    if(!(*rit)->granted_){
      return false;
    }
    auto old_mode = (*rit)->lock_mode_;
    if(old_mode==LockMode::INTENTION_SHARED){
      return true;
    }else if(old_mode==LockMode::INTENTION_EXCLUSIVE &&(new_mode==LockMode::INTENTION_SHARED || new_mode==LockMode::INTENTION_EXCLUSIVE)){
      return true;
    }else if(old_mode==LockMode::SHARED &&(new_mode==LockMode::SHARED || new_mode==LockMode::INTENTION_SHARED)){
      return true;
    }else if(old_mode==LockMode::SHARED_INTENTION_EXCLUSIVE && new_mode==LockMode::INTENTION_SHARED){
      return true;
    }
    break;
  }
  return false;
}
auto LockManager::RowIsLockCompatible(std::shared_ptr<LockRequestQueue> req_queue,LockMode new_mode)const ->bool{
  if(new_mode==LockMode::EXCLUSIVE){
    return false;
  }
  for(auto rit = req_queue->request_queue_.rbegin();rit!=req_queue->request_queue_.rend();++rit){
    if(!(*rit)->granted_){
      return false;
    }
    auto old_mode = (*rit)->lock_mode_;
     if(old_mode==LockMode::SHARED &&new_mode==LockMode::SHARED){
      return true;
    }
    break;
  }
  return false;
}
void LockManager::TableWaitLock(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,std::shared_ptr<LockRequestQueue> req_queue)const{
  auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
  req_queue->request_queue_.emplace_back(new_req);
  std::unique_lock<std::mutex> lck(req_queue->latch_);
  req_queue->cv_.wait(lck);
  new_req->granted_ = true;
  TableTxnLockSetAddRecord(txn, lock_mode, oid);
}
void LockManager::RowWaitLock(Transaction *txn, LockMode lock_mode, const table_oid_t &oid,std::shared_ptr<LockRequestQueue> req_queue,const RID &rid)const{
  auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid,rid);
  req_queue->request_queue_.emplace_back(new_req);
  std::unique_lock<std::mutex> lck(req_queue->latch_);
  req_queue->cv_.wait(lck);
  new_req->granted_ = true;
  RowTxnLockSetAddRecord(txn, lock_mode, oid,rid);
}
auto LockManager::UnlockTableIllegalBehavior(Transaction *txn, const table_oid_t &oid) const -> bool{
  auto s_table_lock_set = txn->GetSharedTableLockSet();
  auto x_table_lock_set = txn->GetExclusiveTableLockSet();
  auto is_table_lock_set = txn->GetIntentionSharedTableLockSet();
  auto ix_table_lock_set = txn->GetIntentionExclusiveTableLockSet();
  auto six_table_lock_set = txn->GetSharedIntentionExclusiveTableLockSet();
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();

  if(s_row_lock_set->find(oid)!=s_row_lock_set->end() || x_row_lock_set->find(oid)!=x_row_lock_set->end()){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::TABLE_UNLOCKED_BEFORE_UNLOCKING_ROWS);
    return true;
  }
  if(s_table_lock_set->find(oid)==s_table_lock_set->end() && x_table_lock_set->find(oid)==x_table_lock_set->end() && is_table_lock_set->find(oid)==is_table_lock_set->end() && ix_table_lock_set->find(oid)==ix_table_lock_set->end() && six_table_lock_set->find(oid)==six_table_lock_set->end()){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}
auto LockManager::UnlockRowIllegalBehavior(Transaction *txn, const table_oid_t &oid,const RID &rid) const -> bool{
  auto s_row_lock_set = txn->GetSharedRowLockSet();
  auto x_row_lock_set = txn->GetExclusiveRowLockSet();
  bool has_s_table_lock = s_row_lock_set->find(oid)!=s_row_lock_set->end();
  bool has_s_row_lock = has_s_table_lock && s_row_lock_set->find(oid)->second.find(rid)!=s_row_lock_set->find(oid)->second.end();
  bool has_x_table_lock = x_row_lock_set->find(oid)!=x_row_lock_set->end();
  bool has_x_row_lock = has_x_table_lock && x_row_lock_set->find(oid)->second.find(rid)!=x_row_lock_set->find(oid)->second.end();
  if(!has_s_row_lock && !has_x_row_lock){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::ATTEMPTED_UNLOCK_BUT_NO_LOCK_HELD);
    return true;
  }
  return false;
}

void LockManager::UpdateTxnState(Transaction *txn, LockMode lock_mode)const{
  if(lock_mode==LockMode::SHARED || lock_mode==LockMode::EXCLUSIVE){
    auto isolation_level = txn->GetIsolationLevel();
    if(isolation_level==IsolationLevel::REPEATABLE_READ){
      txn->SetState(TransactionState::SHRINKING);
    }else if(isolation_level==IsolationLevel::READ_COMMITTED && lock_mode==LockMode::EXCLUSIVE){
      txn->SetState(TransactionState::SHRINKING);
    }else if(isolation_level==IsolationLevel::READ_UNCOMMITTED && lock_mode==LockMode::EXCLUSIVE){
      txn->SetState(TransactionState::SHRINKING);
    }
  }
}
// end of helper function
auto LockManager::LockTable(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("LockTable: txn_id: %d, lock_mode: %d, oid: %d,iso level: %d", txn->GetTransactionId(), int(lock_mode), oid, int(txn->GetIsolationLevel()));
#endif
  if(LockTableIllegalBehavior(txn, lock_mode,oid)){
    return false;
  }
  auto req_queue_itr = table_lock_map_.find(oid);
  if(req_queue_itr == table_lock_map_.end()){
    TableCreateNewReqQueue(txn, lock_mode, oid);
    return true;
  }
  if(TryTableLockUpgrade(txn, req_queue_itr->second, lock_mode)){
    return true;
  }
  
  if(TableIsLockCompatible(req_queue_itr->second,lock_mode)){
    // give the lock
    auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    TableTxnLockSetAddRecord(txn, lock_mode, oid);
    return true;
  } else {
    TableWaitLock(txn, lock_mode, oid, req_queue_itr->second);
  }
  return true;
}

auto LockManager::UnlockTable(Transaction *txn, const table_oid_t &oid) -> bool {
#ifdef Debug
  LOG_INFO("UnLockTable: txn_id: %d,oid: %d,iso level: %d", txn->GetTransactionId(), oid, int(txn->GetIsolationLevel()));
#endif 
  if(UnlockTableIllegalBehavior(txn, oid)){
    return false;
  }
  // update lock table
  LockRequest* txn_req = nullptr;
  auto req_queue_itr = table_lock_map_.find(oid);
  auto req_queue_ptr = req_queue_itr->second;
  for(auto* req:req_queue_ptr->request_queue_){
    if(req->txn_id_ == txn->GetTransactionId()){
      txn_req = req;
      break;
    }
  }
  auto lock_mode = txn_req->lock_mode_;
  UpdateTxnState(txn, lock_mode);
  TableTxnLockSetDeleteRecord(txn, lock_mode, oid);

  auto* last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = last_req->granted_==false;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  if(req_queue_ptr->request_queue_.empty()){
    table_lock_map_.erase(oid);
  }

  if(has_wait_txn){
    req_queue_itr->second->cv_.notify_all();
  }
  // update txn state
  return true;
}

auto LockManager::LockRow(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) -> bool {
#ifdef Debug
  LOG_INFO("LockRow: txn_id: %d, lock_mode: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(), int(lock_mode), oid, rid.ToString().c_str(), int(txn->GetIsolationLevel()));
#endif
  if(LockRowIllegalBehavior(txn, lock_mode, oid, rid)){
    return false;
  }
  auto req_queue_itr = row_lock_map_.find(rid);
  if(req_queue_itr == row_lock_map_.end()){
    RowCreateNewReqQueue(txn, lock_mode, oid, rid);
    return true;
  }
  if(TryTableLockUpgrade(txn, req_queue_itr->second, lock_mode)){
    return true;
  }
  
  if(RowIsLockCompatible(req_queue_itr->second,lock_mode)){
    // give the lock
    auto* new_req = new LockRequest(txn->GetTransactionId(), lock_mode, oid,rid);
    new_req->granted_ = true;
    req_queue_itr->second->request_queue_.emplace_back(new_req);
    RowTxnLockSetAddRecord(txn, lock_mode, oid,rid);
    return true;
  } else {
    RowWaitLock(txn, lock_mode, oid, req_queue_itr->second,rid);
  }
  return true;
}

auto LockManager::UnlockRow(Transaction *txn, const table_oid_t &oid, const RID &rid) -> bool { 
#ifdef Debug
  LOG_INFO("UnLockRow: txn_id: %d, oid: %d,rid: %s,iso level: %d", txn->GetTransactionId(),oid, rid.ToString().c_str(), int(txn->GetIsolationLevel()));
#endif 
  if(UnlockTableIllegalBehavior(txn, oid)){
    return false;
  }
  // update lock table
  LockRequest* txn_req = nullptr;
  auto req_queue_itr = row_lock_map_.find(rid);
  auto req_queue_ptr = req_queue_itr->second;
  for(auto* req:req_queue_ptr->request_queue_){
    if(req->txn_id_ == txn->GetTransactionId()){
      txn_req = req;
      break;
    }
  }
  auto lock_mode = txn_req->lock_mode_;
  UpdateTxnState(txn, lock_mode);
  RowTxnLockSetDeleteRecord(txn, lock_mode, oid,rid);

  auto* last_req = req_queue_ptr->request_queue_.back();
  bool has_wait_txn = last_req->granted_==false;

  req_queue_ptr->request_queue_.remove(txn_req);
  delete txn_req;

  if(req_queue_ptr->request_queue_.empty()){
    row_lock_map_.erase(rid);
  }

  if(has_wait_txn){
    req_queue_itr->second->cv_.notify_all();
  }
  // update txn state
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
