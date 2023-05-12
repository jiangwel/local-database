//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
// update:lrukrplacment mutex
#include "buffer/lru_k_replacer.h"
#include "common/exception.h"
#include "common/logger.h"

#define Frame_Node_pair std::pair<const bustub::frame_id_t, std::shared_ptr<bustub::LRUKNode>>
#define Bckwrd_k_dstnc_grp std::priority_queue<std::pair<size_t, frame_id_t>>
#define MAX_FRAME_ID_T std::numeric_limits<int>::max()
#define MAX_SIZE_T std::numeric_limits<size_t>::max()
#define ADD_ZERO_9_TIME 1000000000

namespace bustub {
// LRUKNode
LRUKNode::LRUKNode(size_t current_timestamp, size_t k) {
  LRUKNode::k_ = k;
  LRUKNode::history_ptr_ = std::make_shared<std::deque<size_t>>(2 * k);
  InsertCurrentTimeStamp(current_timestamp);
}

void LRUKNode::InsertCurrentTimeStamp(size_t current_timestamp) {
  (*LRUKNode::history_ptr_).push_front(current_timestamp);
  LRUKNode::timestamp_num_++;
}

auto LRUKNode::GetIsEvictable() -> bool { return this->is_evictable_; }

auto LRUKNode::GetTimestampNum() -> size_t { return this->timestamp_num_; }

auto LRUKNode::GetHistoryPtr() -> std::shared_ptr<std::deque<size_t>> { return this->history_ptr_; }

auto LRUKNode::GetK() -> size_t { return this->k_; }

void LRUKNode::SetIsEvictable(bool set_evictable_) { this->is_evictable_ = set_evictable_; }

// LRUKReplacer
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) {
  // latch_.lock();
  LRUKReplacer::k_ = k;
  LRUKReplacer::replacer_size_ = num_frames;
  // LOG_INFO("LOGCREAT LRUKReplacer size is: %zu,k is: %zu",num_frames,k);
  LRUKReplacer::node_store_ptr_ = std::make_unique<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>>();
  // latch_.unlock();
}

inline void LRUKReplacer::VctmIsLssThnKTmstmpErlrAccssd(
    const std::shared_ptr<LRUKNode> &frame_p, const std::shared_ptr<std::pair<frame_id_t, size_t>> &evict_frame_p,
    frame_id_t current_frame_id_p) {
  auto first_access_timestamp = frame_p->GetHistoryPtr()->at(frame_p->GetTimestampNum() - 1);
  if (first_access_timestamp <= evict_frame_p->second) {
    evict_frame_p->first = current_frame_id_p;
    evict_frame_p->second = first_access_timestamp;
  }
}

inline auto LRUKReplacer::GetNodeStatus(const std::shared_ptr<LRUKNode> &node_ptr,
                                        const bool *is_evict_had_k_timestamp_node_p,
                                        const std::shared_ptr<std::pair<frame_id_t, size_t>> &evict_frame_p)
    -> LRUKReplacer::NodeStatus {
  NodeStatus ans = NodeStatus::Not_Evictable;
  if (node_ptr->GetIsEvictable()) {
    if (*is_evict_had_k_timestamp_node_p && node_ptr->GetTimestampNum() >= this->k_) {
      ans = NodeStatus::Exst_k_Timestmp;
    } else if (!*is_evict_had_k_timestamp_node_p && node_ptr->GetTimestampNum() < this->k_) {
      ans = NodeStatus::Not_Exst_k_Timestmp;
    } else if (*is_evict_had_k_timestamp_node_p && node_ptr->GetTimestampNum() < this->k_) {
      ans = NodeStatus::Exst_k_Timestmp_Curr_Lessthan_k_Timestmp;
    } else if (!*is_evict_had_k_timestamp_node_p && node_ptr->GetTimestampNum() >= this->k_) {
      ans = NodeStatus::Not_Exst_k_Timestmp_Curr_Morethan_k_Timestmp;
    }
  }
  return ans;
}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  // latch_.lock();

  // first is frame is,second is backward_k_distance
  std::shared_ptr<std::pair<frame_id_t, size_t>> evict_frame_ptr =
      std::make_shared<std::pair<frame_id_t, size_t>>(MAX_FRAME_ID_T, MAX_SIZE_T);
  bool init_bool = true;
  bool *is_evict_had_k_timestamp_node = &init_bool;

  for (const auto &it : *node_store_ptr_) {
    const auto &frame = it.second;
    auto node_status = GetNodeStatus(frame, is_evict_had_k_timestamp_node, evict_frame_ptr);
    auto current_history_ptr = frame->GetHistoryPtr();
    auto current_frame_id = it.first;

    switch (node_status) {
      case NodeStatus::Exst_k_Timestmp: {
        size_t k_timestamp = current_history_ptr->at(this->k_ - 1);
        // LOG_INFO("LOGSTATE Exst_k_Timestmp,backward_k_distance is: %zu",k_timestamp);
        if (k_timestamp < evict_frame_ptr->second) {
          evict_frame_ptr->first = current_frame_id;
          evict_frame_ptr->second = k_timestamp;
        }
        break;
      }
      case NodeStatus::Not_Exst_k_Timestmp: {
        // LOG_INFO("LOGSTATE Not_Exst_k_Timestmp");
        VctmIsLssThnKTmstmpErlrAccssd(frame, evict_frame_ptr, current_frame_id);
        break;
      }
      // this case we don't need consider that k timestmp frame any more.
      case NodeStatus::Exst_k_Timestmp_Curr_Lessthan_k_Timestmp: {
        // LOG_INFO("LOGSTATE Exst_k_Timestmp_Curr_Lessthan_k_Timestmp");
        *is_evict_had_k_timestamp_node = false;
        *evict_frame_ptr = {MAX_FRAME_ID_T, MAX_SIZE_T};
        VctmIsLssThnKTmstmpErlrAccssd(frame, evict_frame_ptr, current_frame_id);
        break;
      }
      case NodeStatus::Not_Exst_k_Timestmp_Curr_Morethan_k_Timestmp:
        // LOG_INFO("LOGSTATE Not_Exst_k_Timestmp_Curr_Morethan_k_Timestmp");
        break;
      case NodeStatus::Not_Evictable: {
        // LOG_INFO("LOGSTATE Not_Evictable");
        break;
      }
    }
  }
  // no evictable frame
  if (evict_frame_ptr->first == MAX_FRAME_ID_T) {
    // latch_.unlock();
    return false;
  }
  *frame_id = evict_frame_ptr->first;
  LRUKReplacer::node_store_ptr_->erase(*frame_id);
  this->curr_size_--;
  // LOG_INFO("Evict frame_id: %d,new size is: %zu", *frame_id, this->curr_size_);
  // delete in future
  // for (const auto &it : *node_store_ptr_) {
  // LOG_INFO("    frame_id: %d ,is_evictable: %d", it.first, it.second->GetIsEvictable());
  // }
  //
  // latch_.unlock();
  return true;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  // latch_.lock();
  // frame id is invaild
  if (frame_id > static_cast<int>(replacer_size_)) {
    // latch_.unlock();
    throw Exception("Frame_id is invaild!");
  }
  // want to insert new frame but buffer pool is fulled.
  if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end() &&
      LRUKReplacer::curr_size_ == LRUKReplacer::replacer_size_) {
    // latch_.unlock();
    throw Exception("Buffer pool is fulled,can't add frame any more!");
  }

  // get nsec level current time
  std::timespec ts;
  timespec_get(&ts, TIME_UTC);
  this->current_timestamp_ = ts.tv_sec * ADD_ZERO_9_TIME + ts.tv_nsec;

  // insert a new frame to buffer pool
  if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end()) {
    // LOG_INFO("Insert a new frame, frame_id: %d", frame_id);
    std::shared_ptr<LRUKNode> node_ptr = std::make_shared<LRUKNode>(this->current_timestamp_, this->k_);
    LRUKReplacer::node_store_ptr_->insert(std::make_pair(frame_id, node_ptr));
    // delete in future
    //  for (const auto &it : *node_store_ptr_) {
    //  LOG_INFO("frame_id: %d ,is_evictable: %d", it.first, it.second->GetIsEvictable());
    // }
    //
  } else {  // update the timestamp of the old frame
    // LOG_INFO("Access old frame, frame_id: %d", frame_id);
    auto frame_ptr = LRUKReplacer::node_store_ptr_->find(frame_id)->second;
    frame_ptr->InsertCurrentTimeStamp(this->current_timestamp_);
  }
  // latch_.unlock();
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  std::scoped_lock<std::mutex> lock(latch_);
  // latch_.lock();
  if (frame_id > static_cast<int>(replacer_size_)) {
    // latch_.unlock();
    throw Exception("Frame_id is invaild!");
  }
  // If the frame_id to be set does not exist, throw an exception
  if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end()) {
    // LOG_DEBUG("LOGBUG Frame_id: %d is not exist!", frame_id);
    // latch_.unlock();
    return;
  }
  // Find the corresponding lruknode through Frame_id,
  // then set its IS_EVICTABLE_ attribute, and finally change the
  // number of current replaceable frames
  auto frame_ptr = LRUKReplacer::node_store_ptr_->find(frame_id)->second;
  if (frame_ptr->GetIsEvictable() && !set_evictable) {
    this->curr_size_--;
  } else if (!frame_ptr->GetIsEvictable() && set_evictable) {
    this->curr_size_++;
  }
  frame_ptr->SetIsEvictable(set_evictable);
  // LOG_INFO("Set frame_id: %d, is_evictable: %d, new size is: %zu", frame_id, set_evictable, this->curr_size_);
  // latch_.unlock();
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  // latch_.lock();
  auto frameid_frame_pair = LRUKReplacer::node_store_ptr_->find(frame_id);
  // frame to be removed does not exist
  if (frameid_frame_pair == LRUKReplacer::node_store_ptr_->end()) {
    // latch_.unlock();
    return;
  }
  if (frameid_frame_pair->second->GetIsEvictable()) {
    LRUKReplacer::node_store_ptr_->erase(frame_id);
    this->curr_size_ -= 1;
    // LOG_INFO("Remove frame_id: %d,new size is: %zu", frame_id, this->curr_size_);
    // delete in future
    // for (const auto &it : *node_store_ptr_) {
    // LOG_INFO("frame_id: %d ,is_evictable: %d", it.first, it.second->GetIsEvictable());
    // }
    //
  } else {
    // latch_.unlock();
    throw Exception("Frame is not evictable!");
  }
  // latch_.unlock();
}

auto LRUKReplacer::Size() -> size_t {
  // LOG_INFO("curr_size_ is %zu",this->curr_size_);
  return this->curr_size_;
}

}  // namespace bustub
// Delete in the future
// auto LRUKReplacer::GetNodeStorePtr() -> std::shared_ptr<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>> {
//   return LRUKReplacer::node_store_ptr_;
// }
