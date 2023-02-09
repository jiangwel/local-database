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

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {
    LRUKNode::LRUKNode(size_t current_timestamp, size_t k) {
        latch_.lock();

        k_ = k;
        timestamp_num_ = 0;
        std::unique_ptr<std::vector<size_t>> temp_ptr(new std::vector<size_t>(2 * k_));
        history_ptr_.reset(temp_ptr.release());
        insertCurrentTimeStamp(current_timestamp);

        latch_.unlock();
    }
    void LRUKNode::insertCurrentTimeStamp(size_t current_timestamp) {
        latch_.lock();
        if (timestamp_num_ < 2 * k_)
            (*history_ptr_)[timestamp_num_++] = current_timestamp;
        else
            (*history_ptr_).push_back(current_timestamp);
        latch_.unlock();
    }
    LRUKNode::~LRUKNode() {
        latch_.lock();
        history_ptr_.reset();
        latch_.unlock();
    }

    LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) {
        latch_.lock();
        k_ = k;
        replacer_size_ = num_frames;
        std::unique_ptr<std::unordered_map<frame_id_t, LRUKNode>> temp_ptr(new std::unordered_map<frame_id_t, LRUKNode>);
        node_store_ptr_.reset(temp_ptr.release());
        latch_.unlock();
    }

    LRUKReplacer::~LRUKReplacer() {
        latch_.lock();
        node_store_ptr_.reset();
        latch_.unlock();
    }

    auto LRUKReplacer::Evict(frame_id_t* frame_id) -> bool { return false; }

    void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {}

    void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {}

    void LRUKReplacer::Remove(frame_id_t frame_id) {}

    auto LRUKReplacer::Size() -> size_t { return 0; }  

}  // namespace bustub
