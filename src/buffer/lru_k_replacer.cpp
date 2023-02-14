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
//定义一个frame_id_t类型的 最大值的宏
#define MAX_FRAME_ID_T std::numeric_limits<bustub::frame_id_t>::max()
//定义一个size_t类型的 最大值的宏
#define MAX_SIZE_T std::numeric_limits<size_t>::max()


namespace bustub {
    //LRUKNode
    LRUKNode::LRUKNode(size_t current_timestamp, size_t k) {
        LRUKNode::latch_.lock();
        //LOG_INFO("@5");
        LRUKNode::k_ = k;
        //LOG_INFO("@6");
        //LOG_INFO("@7");
        //LOG_INFO("@8");
        LRUKNode::history_ptr_ = std::make_shared<std::deque<size_t>>(2 * k);
        //LOG_INFO("@9");
        insertCurrentTimeStamp(current_timestamp);
        //LOG_INFO("@10");
        LRUKNode::latch_.unlock();
    }


    void LRUKNode::insertCurrentTimeStamp(size_t current_timestamp) {
        //LOG_INFO("@11");
        (*LRUKNode::history_ptr_).push_front(current_timestamp);
        LRUKNode::timestamp_num_++;

        //LOG_INFO("@13");
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
    inline auto LRUKReplacer::get_node_status(std::shared_ptr<LRUKNode> node_ptr, bool* exst_k_tmstmp_frm_p, bool* exist_evictable_frame_p, std::pair<frame_id_t, size_t> earliest_access_frame_p) -> LRUKReplacer::NodeStatus {
        if (*exist_evictable_frame_p == false && node_ptr->get_is_evictable() == true) {
            *exist_evictable_frame_p = true;
        }
        if (*exist_evictable_frame_p == true) {
            if (*exst_k_tmstmp_frm_p == true) {
                if (node_ptr->get_timestamp_num() < k_) {
                    return NodeStatus::Evictable_MoreThanKHistories_CurrentLessThanKHistories;
                }
                else {
                    return NodeStatus::Evictable_MoreThanKHistories_CurrentMoreThanKHistories;
                }
            }
            else {
                if (node_ptr->get_timestamp_num() < k_) {
                    auto firsttime_access_timestamp = node_ptr->get_history_ptr()->at(node_ptr->get_timestamp_num() - 1);
                    if (firsttime_access_timestamp < earliest_access_frame_p.second) {
                        return NodeStatus::Evictable_LessThanKHistories_CurrentLessThanKHistories_CurrentMoreEarlier;
                    }
                }
                else {
                    *exst_k_tmstmp_frm_p = true;
                    if (earliest_access_frame_p.first == MAX_FRAME_ID_T) {
                        return NodeStatus::Evictable_LessThanKHistories_CurrentMoreThanKHistories_IsFirstNode;
                    }
                    else {
                        return NodeStatus::Evictable_LessThanKHistories_CurrentMoreThanKHistories_NotFirstNode;
                    }
                }
            }
        }
        return NodeStatus::Null_Status;
    }
    auto LRUKReplacer::Evict(frame_id_t* frame_id) -> bool {
        LRUKReplacer::latch_.lock();
        //no frame or no is_evictable frame
        Bckwrd_k_dstnc_grp bckwrd_k_dstnc_grp;
        std::pair<frame_id_t, size_t> earliest_access_frame = { MAX_FRAME_ID_T,MAX_SIZE_T };
        bool false_value = false;
        bool* exst_k_tmstmp_frm = &false_value;
        bool* exist_evictable_frame = &false_value;
        for (auto it : *node_store_ptr_) {
            auto frame = it.second;
            auto node_status = get_node_status(frame, exst_k_tmstmp_frm, exist_evictable_frame, earliest_access_frame);
            auto current_history_ptr = frame->get_history_ptr();
            auto current_frame_id = it.first;
            switch (node_status) {
            case NodeStatus::Evictable_LessThanKHistories_CurrentMoreThanKHistories_IsFirstNode:
            case NodeStatus::Evictable_MoreThanKHistories_CurrentMoreThanKHistories: {
                auto backward_k_distance = current_history_ptr->at(0) - current_history_ptr->at(this->k_ - 1);
                bckwrd_k_dstnc_grp.push({ backward_k_distance,current_frame_id });
                break;
            }
            case NodeStatus::Evictable_MoreThanKHistories_CurrentLessThanKHistories: {
                *frame_id = current_frame_id;
                LRUKReplacer::node_store_ptr_->erase(*frame_id);
                this->curr_size_--;
                LRUKReplacer::latch_.unlock();
                return true;
            }
            case NodeStatus::Evictable_LessThanKHistories_CurrentMoreThanKHistories_NotFirstNode: {
                auto backward_k_distance = current_history_ptr->at(0) - current_history_ptr->at(this->k_ - 1);
                bckwrd_k_dstnc_grp.push({ backward_k_distance,current_frame_id });
                break;
            }
            case NodeStatus::Evictable_LessThanKHistories_CurrentLessThanKHistories_CurrentMoreEarlier: {
                auto firsttime_access_timestamp = current_history_ptr->at(frame->get_timestamp_num() - 1);
                earliest_access_frame = { current_frame_id,firsttime_access_timestamp };
                break;
            }
            case NodeStatus::Null_Status:
                break;
            }
        }
        //no evictable frame
        if (*exist_evictable_frame == false) {
            LRUKReplacer::latch_.unlock();
            return false;
        }
        if (*exst_k_tmstmp_frm == false) {
            *frame_id = earliest_access_frame.first;
        }
        else {
            *frame_id = bckwrd_k_dstnc_grp.top().second;
        }
        LRUKReplacer::node_store_ptr_->erase(*frame_id);
        this->curr_size_--;
        LRUKReplacer::latch_.unlock();
        return true;
    }

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

    auto LRUKReplacer::get_node_store_ptr()->std::shared_ptr<std::unordered_map<frame_id_t, std::shared_ptr<LRUKNode>>> { return LRUKReplacer::node_store_ptr_; }

}  // namespace bustub
