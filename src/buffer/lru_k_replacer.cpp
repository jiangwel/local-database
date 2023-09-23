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
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : k_(k), replacer_size_(num_frames) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  if (evictable_num_ == 0) {
    LOG_INFO("LRUKReplacer::Evict: evictable_num_ is 0!");
    return false;
  }
  for (auto it = access_less_k_.begin(); it != access_less_k_.end(); it++) {
    if (frame_info_[*it].evictable_) {
      *frame_id = *it;
      frame_info_.erase(*it);
      access_less_k_.remove(*it);
      evictable_num_--;
      return true;
    }
  }
  for (auto it = access_k_frame_.begin(); it != access_k_frame_.end(); it++) {
    if (frame_info_[*it].evictable_) {
      *frame_id = *it;
      frame_info_.erase(*it);
      access_k_frame_.remove(*it);
      evictable_num_--;
      return true;
    }
  }
  LOG_INFO("LRUKReplacer::Evict: no frame can be evicted!");
  return false;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("Can't insert frame any more!");
  }
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    frame_info_[frame_id] = {1, false};
    access_less_k_.push_back(frame_id);
    return;
  }
  ++frame_info_[frame_id].access_time_;
  if (frame_info_[frame_id].access_time_ == k_) {
    access_k_frame_.push_back(frame_id);
    access_less_k_.remove(frame_id);
  } else if (frame_info_[frame_id].access_time_ > k_) {
    access_k_frame_.remove(frame_id);
    access_k_frame_.push_back(frame_id);
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("SetEvictable:frame_id is invalid!");
  }
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    return;
  }
  if (set_evictable != frame_info_[frame_id].evictable_) {
    frame_info_[frame_id].evictable_ = set_evictable;
    evictable_num_ += set_evictable ? 1 : -1;
  }
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw Exception("Remove:frame_id is invalid!");
  }
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    return;
  }
  if (frame_info_[frame_id].evictable_) {
    std::list<frame_id_t> &frame_id_list = frame_info_[frame_id].access_time_ < k_ ? access_less_k_ : access_k_frame_;
    frame_id_list.remove(frame_id);
    frame_info_.erase(frame_id);
    evictable_num_--;
  } else {
    throw Exception("Remove:frame_id is not evictable!");
  }
}

auto LRUKReplacer::Size() -> size_t { return this->evictable_num_; }

}  // namespace bustub
