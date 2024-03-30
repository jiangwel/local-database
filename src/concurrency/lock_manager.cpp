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
// helper function
auto LockManager::IllegalBehaviorTable(Transaction *txn, LockMode lock_mode) -> bool{
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
  } else if((isolation_level==IsolationLevel::REPEATABLE_READ || isolation_level==IsolationLevel::READ_UNCOMMITTED) && txn_state==TransactionState::SHRINKING){
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

auto LockManager::LockTable(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) -> bool {
  if(IllegalBehaviorTable(txn, lock_mode)){
    return false;
  }
  // auto req_queue_itr = table_lock_map_.find(oid);
  // if(req_queue_itr == table_lock_map_.end()){
  //   // auto* new_request = new LockRequest(txn->GetTransactionId(), lock_mode, oid);
  //   // new_request->granted_ = true;
  //   // auto new_req_queue = std::make_shared<LockRequestQueue>(new LockRequestQueue(new_request));
  //   // table_lock_map_[oid] = new_req_queue;
  //   // CreateNewReqQueueTable(txn, lock_mode, oid);
  //   UpdateTxnLockSetTable(txn, oid, lock_mode);
  //   return true;
  // }
  // for(auto* req:req_queue_itr->second->request_queue_){
  //   if(req->txn_id_ == txn->GetTransactionId() && req->granted_){
  //     return LockUpgradeTable(txn, lock_mode, oid);
  //   }
  // }
  // if(IsLockCompatible()){
  //   // give the lock
  // }
  // WaitLock();
  return true;
}

auto LockManager::UnlockTable(Transaction *txn, const table_oid_t &oid) -> bool { return true; }

auto LockManager::LockRow(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) -> bool {
  return true;
}

auto LockManager::UnlockRow(Transaction *txn, const table_oid_t &oid, const RID &rid) -> bool { return true; }

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
