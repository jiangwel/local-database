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
#include "common/logger.h"

#define Frame_Node_pair std::pair<const bustub::frame_id_t, std::shared_ptr<bustub::LRUKNode>>
#define Bckwrd_k_dstnc_grp std::priority_queue<std::pair<size_t, frame_id_t>>
#define MAX_FRAME_ID_T std::numeric_limits<int>::max()
#define MAX_SIZE_T std::numeric_limits<size_t>::max()


namespace bustub {
    //LRUKNode
    LRUKNode::LRUKNode(size_t current_timestamp, size_t k) {
        LRUKNode::latch_.lock();
        LRUKNode::k_ = k;
        LRUKNode::history_ptr_ = std::make_shared<std::deque<size_t>>(2 * k);
        insertCurrentTimeStamp(current_timestamp);
        LRUKNode::latch_.unlock();
    }

    void LRUKNode::insertCurrentTimeStamp(size_t current_timestamp) {
        (*LRUKNode::history_ptr_).push_front(current_timestamp);
        LRUKNode::timestamp_num_++;
    }

    void LRUKNode::printHistory() {
        for (auto i : *history_ptr_) {
            std::cout << " " << i << " ";
        }
    }

    auto LRUKNode::get_is_evictable()->bool { return this->is_evictable_; }

    auto LRUKNode::get_timestamp_num()->size_t { return this->timestamp_num_; }

    auto LRUKNode::get_history_ptr()->std::shared_ptr<std::deque<size_t>> { return this->history_ptr_; }

    auto LRUKNode::get_k()->size_t { return this->k_; }

    void LRUKNode::set_is_evictable(bool set_evictable_) { this->is_evictable_ = set_evictable_; }

    //LRUKReplacer
    LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) {
        LRUKReplacer::latch_.lock();
        LRUKReplacer::k_ = k;
        LRUKReplacer::replacer_size_ = num_frames;
        LRUKReplacer::node_store_ptr_ = std::make_unique<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>>();
        LRUKReplacer::latch_.unlock();
    }

    inline void LRUKReplacer::vctm_is_lss_thn_k_tmstmp_erlr_accssd(std::shared_ptr<LRUKNode> frame_p, std::shared_ptr<std::pair<frame_id_t, size_t>> evict_frame_p, frame_id_t current_frame_id_p) {
        auto first_access_timestamp = frame_p->get_history_ptr()->at(frame_p->get_timestamp_num() - 1);
        if (first_access_timestamp <= evict_frame_p->second) {
            evict_frame_p->first = current_frame_id_p;
            evict_frame_p->second = first_access_timestamp;
        }
    }

    inline auto LRUKReplacer::get_node_status(std::shared_ptr<LRUKNode> node_ptr, bool* is_evict_had_k_timestamp_node_p, std::shared_ptr<std::pair<frame_id_t, size_t>> evict_frame_p) -> LRUKReplacer::NodeStatus {
        if (node_ptr->get_is_evictable() == true) {
            if (*is_evict_had_k_timestamp_node_p == true && node_ptr->get_timestamp_num() >= this->k_) {
                return NodeStatus::Exst_k_Tmstmp_Node;
            }
            else if (*is_evict_had_k_timestamp_node_p == false && node_ptr->get_timestamp_num() < this->k_) {
                return NodeStatus::Not_Exst_k_Tmstmp_Node;
            }
            else if (*is_evict_had_k_timestamp_node_p == true && node_ptr->get_timestamp_num() < this->k_) {
                return NodeStatus::Exst_k_Tmstmp_Node_Curr_Lessthan_k_Tmstmp;
            }
            else if (*is_evict_had_k_timestamp_node_p == false && node_ptr->get_timestamp_num() >= this->k_) {
                return NodeStatus::Not_Exst_k_Tmstmp_Node_Curr_Morethan_k_Tmstmp;
            }
        }
        return NodeStatus::Not_Evictable;
    }
    auto LRUKReplacer::Evict(frame_id_t* frame_id) -> bool {
        LRUKReplacer::latch_.lock();
        //no frame or no is_evictable frame
        std::shared_ptr<std::pair<frame_id_t, size_t>> evict_frame_ptr = std::make_shared<std::pair<frame_id_t, size_t> >(MAX_FRAME_ID_T, 0);
        bool init_bool_ptr = true;
        bool* is_evict_had_k_timestamp_node = &init_bool_ptr;

        for (auto it : *node_store_ptr_) {
            auto frame = it.second;
            auto node_status = get_node_status(frame, is_evict_had_k_timestamp_node, evict_frame_ptr);
            auto current_history_ptr = frame->get_history_ptr();
            auto current_frame_id = it.first;
            switch (node_status) {
            case NodeStatus::Exst_k_Tmstmp_Node: {
                auto backward_k_distance = current_history_ptr->at(0) - current_history_ptr->at(this->k_ - 1);
                if (backward_k_distance > evict_frame_ptr->second) {
                    evict_frame_ptr->first = current_frame_id;
                    evict_frame_ptr->second = backward_k_distance;
                }
                break;
            }
            case NodeStatus::Not_Exst_k_Tmstmp_Node: {
                vctm_is_lss_thn_k_tmstmp_erlr_accssd(frame, evict_frame_ptr, current_frame_id);
                break;
            }
            case NodeStatus::Exst_k_Tmstmp_Node_Curr_Lessthan_k_Tmstmp: {
                *is_evict_had_k_timestamp_node = false;
                *evict_frame_ptr = { MAX_FRAME_ID_T,MAX_SIZE_T };
                vctm_is_lss_thn_k_tmstmp_erlr_accssd(frame, evict_frame_ptr, current_frame_id);
                break;
            }
            case NodeStatus::Not_Exst_k_Tmstmp_Node_Curr_Morethan_k_Tmstmp: {
                break;
            }
            case NodeStatus::Not_Evictable: {
                break;
            }
            }
        }
        //no evictable frame
        if (evict_frame_ptr->first == MAX_FRAME_ID_T) {
            LRUKReplacer::latch_.unlock();
            return false;
        }
        *frame_id = evict_frame_ptr->first;
        LRUKReplacer::node_store_ptr_->erase(*frame_id);
        this->curr_size_--;
        LRUKReplacer::latch_.unlock();
        return true;
    }
    //Delete in the future
    void LRUKReplacer::printRecord(frame_id_t frame_id) {
        std::cout << frame_id << " ";
        auto frame_ptr = node_store_ptr_->find(frame_id)->second;
        frame_ptr->printHistory();
        std::cout << std::endl;
    }

    void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
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
        }
        //a record fo access old frame
        else {
            auto frame_ptr = LRUKReplacer::node_store_ptr_->find(frame_id)->second;
            frame_ptr->insertCurrentTimeStamp(this->current_timestamp_);
        }
        LRUKReplacer::latch_.unlock();
    }

    void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
        //If the frame_id to be set does not exist, throw an exception
        if (LRUKReplacer::node_store_ptr_->find(frame_id) == LRUKReplacer::node_store_ptr_->end()) {
            throw Exception("Frame_id is not exist!");
        }
        LRUKReplacer::latch_.lock();
        //Find the corresponding lruknode through Frame_id, then set its IS_EVICTABLE_ attribute, and finally change the number of current replaceable frames
        auto frame_ptr = LRUKReplacer::node_store_ptr_->find(frame_id)->second;
        if (frame_ptr->get_is_evictable() == true && set_evictable == false) {
            this->curr_size_--;
        }
        else if (frame_ptr->get_is_evictable() == false && set_evictable == true) {
            this->curr_size_++;
        }
        frame_ptr->set_is_evictable(set_evictable);
        //Use the three yuan expression. If set_evictable is true, the current replaceable frame number is added, otherwise one will be reduced by one
        LRUKReplacer::latch_.unlock();
    }

    void LRUKReplacer::Remove(frame_id_t frame_id) {
        auto frameid_frame_pair = LRUKReplacer::node_store_ptr_->find(frame_id);
        //如果找不到frame_id对应的LRUKNode，退出函数
        if (frameid_frame_pair == LRUKReplacer::node_store_ptr_->end()) {
            return;
        }
        LRUKReplacer::latch_.lock();
        //如果frame_id对应的LRUKNode是不可替换的，抛出异常
        if (frameid_frame_pair->second->get_is_evictable() == false) {
            throw Exception("Frame is not evictable!");
        }
        else {
            LRUKReplacer::node_store_ptr_->erase(frame_id);
            this->curr_size_ -= 1;
        }
        LRUKReplacer::latch_.unlock();
    }

    auto LRUKReplacer::Size() -> size_t { return this->curr_size_; }
    //Delete in the future
    auto LRUKReplacer::get_node_store_ptr()->std::shared_ptr<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>> { return LRUKReplacer::node_store_ptr_; }

}  // namespace bustub
