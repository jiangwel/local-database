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
    //LRUKNode
    LRUKNode::LRUKNode(size_t current_timestamp, size_t k) {
        LRUKNode::latch_.lock();

        LRUKNode::k_ = k;
        LRUKNode::timestamp_num_ = 0;
        std::unique_ptr<std::vector<size_t>> temp_ptr(new std::vector<size_t>(2 * k_));
        LRUKNode::history_ptr_.reset(temp_ptr.release());
        insertCurrentTimeStamp(current_timestamp);

        LRUKNode::latch_.unlock();
    }

    LRUKNode::~LRUKNode() {
        LRUKNode::latch_.lock();
        LRUKNode::history_ptr_.reset();
        LRUKNode::latch_.unlock();
    }

    void LRUKNode::insertCurrentTimeStamp(size_t current_timestamp) {
        LRUKNode::latch_.lock();
        if (LRUKNode::timestamp_num_ < 2 * k_)
            (*LRUKNode::history_ptr_)[LRUKNode::timestamp_num_++] = current_timestamp;
        else
            (*LRUKNode::history_ptr_).push_back(current_timestamp);
        LRUKNode::latch_.unlock();
    }

    //LRUKReplacer
    LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) {
        LRUKReplacer::latch_.lock();
        LRUKReplacer::k_ = k;
        LRUKReplacer::replacer_size_ = num_frames;
        std::unique_ptr<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>> temp_ptr(new std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>);
        LRUKReplacer::node_store_ptr_.reset(temp_ptr.release());
        LRUKReplacer::latch_.unlock();
    }

    LRUKReplacer::~LRUKReplacer() {
        LRUKReplacer::latch_.lock();
        LRUKReplacer::node_store_ptr_.reset();
        LRUKReplacer::latch_.unlock();
    }

    auto LRUKReplacer::Evict(frame_id_t* frame_id) -> bool { return false; }

    void LRUKReplacer::RecordAccess(frame_id_t frame_id, AccessType access_type) {
        //want to insert new frame but buffer pool is fulled.
        if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end() && LRUKReplacer::curr_size_ == LRUKReplacer::replacer_size_) {
            throw Exception("Buffer pool is fulled,can't add frame any more!");
        }
        LRUKReplacer::latch_.lock();
        this->current_timestamp_ = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //insert a new frame to buffer pool
        if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end()) {
            std::shared_ptr<LRUKNode> node_ptr = std::make_shared<LRUKNode>(this->current_timestamp_, this->k_);
            LRUKReplacer::node_store_ptr_->insert(std::make_pair(frame_id, node_ptr));
            ++this->curr_size_;
        }
        //a record fo access old frame
        else {
            auto frame_ptr = LRUKReplacer::node_store_ptr_->find(frame_id)->second;
            frame_ptr->insertCurrentTimeStamp(this->current_timestamp_);
        }
        LRUKReplacer::latch_.unlock();
    }

    void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
    }

    void LRUKReplacer::Remove(frame_id_t frame_id) {}

    auto LRUKReplacer::Size() -> size_t { return this->curr_size_; }

}  // namespace bustub
