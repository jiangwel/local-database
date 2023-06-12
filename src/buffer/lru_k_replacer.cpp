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

namespace bustub {
// LRUKReplacer
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : k_(k), replacer_size_(num_frames) {
  LOG_INFO("LRUKReplacer: replacer_size_: %zu, k: %zu", num_frames, k);
}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  // LOG_INFO("Evict");
  if (evictable_num_ == 0) {
    return false;
  }
    // delete
    //  if(access_less_k_.empty()){
    //    // LOG_INFO("  access_k_frame_!");
    //  } else {
    //    // LOG_INFO("  access_less_k_!");
    //  }
  std::list<frame_id_t> &frame_id_list = access_less_k_.empty() ? access_k_frame_ : access_less_k_;
  for (auto it = frame_id_list.rbegin(); it != frame_id_list.rend(); it++) {
    if (frame_info_[*it].evictable_) {
      *frame_id = *it;
      frame_info_.erase(*it);
      frame_id_list.remove(*it);
      evictable_num_--;
      // delete
      //  for(auto it = frame_id_list.begin(); it != frame_id_list.end(); it++){
      //    // LOG_INFO("    %d", *it);
      //  }
      //  // LOG_INFO("  frame_info_");
      //  for(auto it = frame_info_.begin(); it != frame_info_.end(); it++){
      //    // LOG_INFO("    id: %d,access time: %zu,evictable: %d ", it->first, it->second.access_time_,
      //    it->second.evictable_);
      //  }
      return true;
    }
  }
  return false;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  if(frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("Can't insert frame any more!");
  }
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    if (frame_info_.count(frame_id) == 0U){
      frame_info_[frame_id] = {1, false};
      access_less_k_.push_front(frame_id);
      return;
    }
  }
  ++frame_info_[frame_id].access_time_;
  if (frame_info_[frame_id].access_time_ == k_) {
    access_k_frame_.push_front(frame_id);
    access_less_k_.remove(frame_id);
  } else if(frame_info_[frame_id].access_time_ > k_){
    if (frame_info_.count(frame_id) != 0U) {
      access_k_frame_.remove(frame_id);
    }
    access_k_frame_.push_front(frame_id);
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  std::scoped_lock<std::mutex> lock(latch_);
  // LOG_INFO("SetEvictable: frame_id: %d, set_evictable: %d", frame_id, set_evictable);
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("SetEvictable:frame_id is invalid!");
  }
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    throw Exception("SetEvictable: frame_id is invalid!");
  }
  if (set_evictable != frame_info_[frame_id].evictable_) {
    frame_info_[frame_id].evictable_ = set_evictable;
    evictable_num_ += set_evictable ? 1 : -1;
  }
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  // LOG_INFO("Remove: frame_id: %d", frame_id);
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("Remove:frame_id is invalid!");
  }
  if(frame_info_.find(frame_id) == frame_info_.end()) {
    return;
  }
  if (frame_info_[frame_id].evictable_) {
    std::list<frame_id_t> &frame_id_list = frame_info_[frame_id].access_time_ < k_ ? access_less_k_ : access_k_frame_;
    frame_id_list.remove(frame_id);
    frame_info_.erase(frame_id);
    evictable_num_--;
  } else {
    throw Exception("frame_id is not evictable!");
  }
}

auto LRUKReplacer::Size() -> size_t { return this->evictable_num_; }

}  // namespace bustub
