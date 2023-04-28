//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager_instance.h"

#include "common/exception.h"
#include "common/logger.h"
#include "common/macros.h"

#define INVALID_FRAME_ID -1

namespace bustub {

BufferPoolManagerInstance::BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager, size_t replacer_k,
                                                     LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // we allocate a consecutive memory space for the buffer pool
  pages_ = new Page[pool_size_];
  page_table_ = new ExtendibleHashTable<page_id_t, frame_id_t>(bucket_size_);
  replacer_ = new LRUKReplacer(pool_size, replacer_k);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManagerInstance::~BufferPoolManagerInstance() {
  delete[] pages_;
  delete page_table_;
  delete replacer_;
}

auto BufferPoolManagerInstance::NewPgImp(page_id_t *page_id) -> Page * {
  latch_.lock();
  Page *temp_page_ptr = nullptr;
  Page **page_ptr = &temp_page_ptr;
  std::shared_ptr<frame_id_t> frame_id_ptr = std::make_shared<frame_id_t>();
  if (free_list_.empty()) {
    if (GetReplacementPage(frame_id_ptr.get(), page_ptr)) {
      (*page_ptr)->page_id_ = next_page_id_++;
      *page_id = (*page_ptr)->GetPageId();
    } else {
      latch_.unlock();
      return nullptr;
    }
  } else {
    *frame_id_ptr = free_list_.front();
    free_list_.pop_front();
    *page_id = AllocatePage();
    *page_ptr = GetPageFromePageId(*page_id);
    if(*page_ptr==nullptr){
      LOG_DEBUG("[NewPgImp()] page_id %d not found in page_table_", *page_id);
    }// end if
  }  // end else
  latch_.unlock();
  page_table_->Insert(*page_id, *frame_id_ptr);
  latch_.lock();
  ResetPage(*page_ptr, *frame_id_ptr);
  (*page_ptr)->pin_count_++;
  latch_.unlock();
  return *page_ptr;
}  // end NewPgImp

auto BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) -> Page * {
  latch_.lock();
  Page *temp_ptr = nullptr;
  Page **page_ptr_ptr = &temp_ptr;
  frame_id_t frame_id = INVALID_FRAME_ID;
  latch_.unlock();
  // Find page in buffer pool
  page_table_->Find(page_id, frame_id);
  latch_.lock();
  *page_ptr_ptr = GetPageFromePageId(page_id);
  // Find page in buffer pool successfully
  if(*page_ptr_ptr!=nullptr){
    replacer_->SetEvictable(frame_id, false);
    replacer_->RecordAccess(frame_id);
  } else { // Find page in buffer pool failed
  // Get page by evicting other page
    if (free_list_.empty()) {
      if (!GetReplacementPage(&frame_id, page_ptr_ptr)) {
        return nullptr;
      }
    } else { // Get page from free list
      frame_id = free_list_.front();
      free_list_.pop_front();
      *page_ptr_ptr = GetPageFromePageId(INVALID_PAGE_ID);
      if(*page_ptr_ptr==nullptr){
        LOG_DEBUG("[FetchPgImp()] No blank page in buffer pool");
      }
    }  // end else
    // Must steps when get page
    ResetPage(*page_ptr_ptr, frame_id);
    (*page_ptr_ptr)->page_id_ = page_id;
    latch_.unlock();
    page_table_->Insert(page_id, frame_id);
    latch_.lock();
    char *new_page_data = new char[BUSTUB_PAGE_SIZE];
    latch_.unlock();
    disk_manager_->ReadPage(page_id, new_page_data);
    latch_.lock();
    memcpy((*page_ptr_ptr)->GetData(), new_page_data, BUSTUB_PAGE_SIZE);
    delete[] new_page_data;
  }  // end if
  (*page_ptr_ptr)->pin_count_++;
  latch_.unlock();
  return *page_ptr_ptr;
}  // end FetchPgImp

auto BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  latch_.lock();
  auto page_ptr = GetPageFromePageId(page_id);
  if (page_ptr == nullptr) {
    return false;
  }// end if
  if(page_ptr->pin_count_==0){
    LOG_DEBUG("[UnpinPgImp()] page_id %d pin_count_ is 0", page_id);
  }// end if
  page_ptr->pin_count_--;
  frame_id_t frame_id;
  if(!page_table_->Find(page_id, frame_id)){
    LOG_DEBUG("[UnpinPgImp()] page_id %d not found in page_table_", page_id);
  }// end if
  if (page_ptr->pin_count_ == 0) {
    latch_.unlock();
    replacer_->SetEvictable(frame_id, true);
    latch_.lock();
  }// end if
  if (is_dirty) {
    page_ptr->is_dirty_ = true;
  }// end if
  latch_.lock();
  return true;
}  // end UnpinPgImp

auto BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) -> bool {
  frame_id_t frame_id;
  if (page_table_->Find(page_id, frame_id)) {
    latch_.lock();
    auto page = GetPageFromePageId(page_id);
    if(page==nullptr){
      LOG_DEBUG("[FlushPgImp()] page_id %d not found in page_table_", page_id);
    }// end if
    latch_.unlock();
    disk_manager_->WritePage(page_id, page->GetData());
    latch_.lock();
    page->is_dirty_ = false;
    latch_.unlock();
    return true;
  }  // end if
  return false;
}  // end FlushPgImp

void BufferPoolManagerInstance::FlushAllPgsImp() {
  latch_.lock();
  for (size_t i = 0; i < pool_size_; ++i) {
    if (pages_[i].page_id_ != INVALID_PAGE_ID) {
      latch_.unlock();
      FlushPgImp(pages_[i].page_id_);
      latch_.lock();
    }
  }  // end for
  latch_.unlock();
}  // end FlushAllPgsImp

// Delete a page from the buffer pool. If page_id is not in the buffer pool, do nothing and return true. If the
// page is pinned and cannot be deleted, return false immediately.
auto BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) -> bool {
  latch_.lock();
  auto page_ptr = GetPageFromePageId(page_id);
  if (page_ptr == nullptr) {
    latch_.unlock();
    return true;
  }
  if (page_ptr->pin_count_ > 0) {
    latch_.unlock();
    return false;
  }
  frame_id_t framd_id;
  latch_.unlock();
  page_table_->Find(page_id, framd_id);
  latch_.lock();
  DeallocatePage(page_id);
  page_ptr->is_dirty_ = false;
  page_ptr->ResetMemory();
  page_ptr->page_id_ = INVALID_PAGE_ID;
  page_table_->Remove(page_id);
  replacer_->Remove(framd_id);
  free_list_.push_back(framd_id);
  latch_.unlock();
  return true;
}  // end DeletePgImp

// Find blank page in page_ , initialize it's id
auto BufferPoolManagerInstance::AllocatePage() -> page_id_t {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  auto page_ptr = GetPageFromePageId(INVALID_PAGE_ID);
  if(page_ptr==nullptr){
    LOG_DEBUG("[AllocatePage()] Can't get page from page_id %d", INVALID_PAGE_ID);
  }
  page_ptr->page_id_ = next_page_id_;
  return next_page_id_++;
}  // end AllocatePage

void BufferPoolManagerInstance::ResetPage(Page *page, frame_id_t frame_id) {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  if(page==nullptr){
    LOG_DEBUG("[ResetPage()] page is nullptr");
  }
  page->pin_count_ = 0;
  page->is_dirty_ = false;
  replacer_->RecordAccess(frame_id);
  replacer_->SetEvictable(frame_id, false);
}  // end ResetPage

auto BufferPoolManagerInstance::GetPageFromFrameId(frame_id_t frame_id) -> Page * {
  frame_id_t temp_frame_id = 0;
  Page *page_ptr = nullptr;
  size_t i = 0;
  for (; i < pool_size_; ++i) {
    page_table_->Find(pages_[i].page_id_, temp_frame_id);
    if (temp_frame_id == frame_id) break;
  }    // end for
  if(i == pool_size_) {
    LOG_DEBUG("Frame id %d not found in page table", frame_id);
  } else {
    page_ptr = &pages_[i];
  }
  return page_ptr;
}  // end GetPageFromFrameId

auto BufferPoolManagerInstance::GetReplacementPage(frame_id_t *frame_id_ptr, Page **page_ptr) -> bool {
  if (replacer_->Evict(frame_id_ptr)) {
    *page_ptr = GetPageFromFrameId(*frame_id_ptr);
    if ((*page_ptr)->IsDirty()) {
      disk_manager_->WritePage((*page_ptr)->GetPageId(), (*page_ptr)->GetData());
    }
    page_table_->Remove((*page_ptr)->GetPageId());
    return true;
  }  // end if
  return false;
}  // end GetReplacementPage

auto BufferPoolManagerInstance::GetPageFromePageId(page_id_t page_id) -> Page * {
  for (size_t i = 0; i < pool_size_; ++i) {
    if (pages_[i].page_id_ == page_id) {
      return &pages_[i];
    }
  }  // end for
  return nullptr;
}  // end GetPageFromePageId
}  // namespace bustub
