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
  if (evictable_num_ != 0) {
    std::list<frame_id_t> &frame_id_list = access_less_k_.empty() ? access_k_frame_ : access_less_k_;
    for (auto it = frame_id_list.begin(); it != frame_id_list.end(); it++) {
      if (frame_info_[*it].evictable_) {
        *frame_id = *it;
        frame_info_.erase(*it);
        frame_id_list.erase(it);
        evictable_num_--;
        return true;
      }
    }
  }
  return false;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    if (access_k_frame_.size() + access_less_k_.size() < replacer_size_) {
      frame_info_[frame_id] = {1, false};
      access_less_k_.push_back(frame_id);
    } else {
      throw Exception("Can't insert frame any more!");
    }
  } else {
    if (frame_info_[frame_id].access_time_ < k_) {
      if (++frame_info_[frame_id].access_time_ == k_) {
        access_k_frame_.push_back(frame_id);
        access_less_k_.remove(frame_id);
      }
    } else {
      access_k_frame_.remove(frame_id);
      access_k_frame_.push_back(frame_id);
    }
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_info_.find(frame_id) == frame_info_.end()) {
    throw Exception("frame_id is invalid!");
  }
  if (set_evictable != frame_info_[frame_id].evictable_) {
    frame_info_[frame_id].evictable_ = set_evictable;
    evictable_num_ += set_evictable ? 1 : -1;
  }
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);
  if (frame_info_.find(frame_id) != frame_info_.end()) {
    if (frame_info_[frame_id].evictable_) {
      frame_info_.erase(frame_id);
      evictable_num_--;
    } else {
      throw Exception("frame_id is not evictable!");
    }
  }
}

auto LRUKReplacer::Size() -> size_t { return this->evictable_num_; }

}  // namespace bustub
