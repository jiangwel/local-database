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

#define INVALID_FRAME_ID (-1)

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
  std::scoped_lock<std::mutex> lock(latch_);
  Page *temp_page_ptr = nullptr;
  Page **page_ptr = &temp_page_ptr;
  std::shared_ptr<frame_id_t> frame_id_ptr = std::make_shared<frame_id_t>();
  if (free_list_.empty()) {
    if (GetReplacementPage(frame_id_ptr.get(), page_ptr)) {
      (*page_ptr)->page_id_ = next_page_id_++;
      *page_id = (*page_ptr)->GetPageId();
    } else {
      return nullptr;
    }
  } else {
    *frame_id_ptr = free_list_.front();
    free_list_.pop_front();
    *page_id = AllocatePage();
    *page_ptr = &pages_[*frame_id_ptr];
    (*page_ptr)->page_id_ = *page_id;
    // if(*page_ptr==nullptr){
    //   LOG_DEBUG("[NewPgImp()] page_id %d not found in page_table_", *page_id);
    // }// end if
  }  // end else
  page_table_->Insert(*page_id, *frame_id_ptr);
  ResetPage(*page_ptr, *frame_id_ptr);
  (*page_ptr)->pin_count_++;
  return *page_ptr;
}  // end NewPgImp

auto BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) -> Page * {
  std::scoped_lock<std::mutex> lock(latch_);
  Page *temp_ptr = nullptr;
  Page **page_ptr_ptr = &temp_ptr;
  frame_id_t frame_id = INVALID_FRAME_ID;
  // Find page in buffer pool successfully
  if (page_table_->Find(page_id, frame_id)) {
    LOG_INFO("FetchPgImp");
    *page_ptr_ptr = &pages_[frame_id];
    replacer_->SetEvictable(frame_id, false);
    replacer_->RecordAccess(frame_id);
    (*page_ptr_ptr)->pin_count_++;
    return *page_ptr_ptr;
  }
  // Find page in buffer pool failed
  // Get page by evicting other page
  if (free_list_.empty() && !GetReplacementPage(&frame_id, page_ptr_ptr)) {
    return nullptr;
  }
  if (!free_list_.empty()) {  // Get page from free list
    LOG_INFO("hhh");
    frame_id = free_list_.front();
    free_list_.pop_front();
    *page_ptr_ptr = &pages_[frame_id];
    // if(*page_ptr_ptr==nullptr){
    //   LOG_DEBUG("[FetchPgImp()] No blank page in buffer pool");
    // }
  }  // end if
  // Must steps when get page
  ResetPage(*page_ptr_ptr, frame_id);
  (*page_ptr_ptr)->page_id_ = page_id;
  page_table_->Insert(page_id, frame_id);
  char *new_page_data = new char[BUSTUB_PAGE_SIZE];
  disk_manager_->ReadPage(page_id, new_page_data);
  memcpy((*page_ptr_ptr)->GetData(), new_page_data, BUSTUB_PAGE_SIZE);
  delete[] new_page_data;
  (*page_ptr_ptr)->pin_count_++;
  return *page_ptr_ptr;
}  // end FetchPgImp

auto BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  frame_id_t frame_id;

  if (!page_table_->Find(page_id, frame_id)) {
    return false;
  }  // end if
  auto page_ptr = &pages_[frame_id];
  // if(page_ptr->pin_count_==0){
  //   LOG_DEBUG("[UnpinPgImp()] page_id %d pin_count_ is 0", page_id);
  // }// end if
  page_ptr->pin_count_--;

  // if(!page_table_->Find(page_id, frame_id)){
  //   LOG_DEBUG("[UnpinPgImp()] page_id %d not found in page_table_", page_id);
  // }// end if
  if (page_ptr->pin_count_ == 0) {
    replacer_->SetEvictable(frame_id, true);
  }  // end if
  if (is_dirty) {
    page_ptr->is_dirty_ = true;
  }  // end if
  return true;
}  // end UnpinPgImp

auto BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  frame_id_t frame_id;
  if (page_table_->Find(page_id, frame_id)) {
    auto page = &pages_[frame_id];
    // if(page==nullptr){
    //   LOG_DEBUG("[FlushPgImp()] page_id %d not found in page_table_", page_id);
    // }// end if
    disk_manager_->WritePage(page_id, page->GetData());
    page->is_dirty_ = false;
    return true;
  }  // end if
  return false;
}  // end FlushPgImp

void BufferPoolManagerInstance::FlushAllPgsImp() {
  std::scoped_lock<std::mutex> lock(latch_);
  for (size_t i = 0; i < pool_size_; ++i) {
    if (pages_[i].page_id_ != INVALID_PAGE_ID) {
      std::scoped_lock<std::mutex> unlock(latch_);
      FlushPgImp(pages_[i].page_id_);
      std::scoped_lock<std::mutex> lock(latch_);
    }
  }  // end for
}  // end FlushAllPgsImp

// Delete a page from the buffer pool. If page_id is not in the buffer pool, do nothing and return true. If the
// page is pinned and cannot be deleted, return false immediately.
auto BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  frame_id_t frame_id;
  if (!page_table_->Find(page_id, frame_id)) {
    return true;
  }
  auto page_ptr = &pages_[frame_id];
  if (page_ptr->pin_count_ > 0) {
    return false;
  }

  DeallocatePage(page_id);
  page_ptr->is_dirty_ = false;
  page_ptr->ResetMemory();
  page_ptr->page_id_ = INVALID_PAGE_ID;
  page_table_->Remove(page_id);
  replacer_->Remove(frame_id);
  free_list_.push_back(frame_id);
  return true;
}  // end DeletePgImp

// Find blank page in page_ , initialize it's id
auto BufferPoolManagerInstance::AllocatePage() -> page_id_t { return next_page_id_++; }  // end AllocatePage

void BufferPoolManagerInstance::ResetPage(Page *page, frame_id_t frame_id) {
  // if(page==nullptr){
  //   LOG_DEBUG("[ResetPage()] page is nullptr");
  // }
  page->pin_count_ = 0;
  page->is_dirty_ = false;
  replacer_->RecordAccess(frame_id);
  replacer_->SetEvictable(frame_id, false);
}  // end ResetPage

auto BufferPoolManagerInstance::GetReplacementPage(frame_id_t *frame_id_ptr, Page **page_ptr) -> bool {
  if (replacer_->Evict(frame_id_ptr)) {
    *page_ptr = &pages_[*frame_id_ptr];
    if ((*page_ptr)->IsDirty()) {
      disk_manager_->WritePage((*page_ptr)->GetPageId(), (*page_ptr)->GetData());
    }
    if (!page_table_->Remove((*page_ptr)->GetPageId())) {
      LOG_DEBUG("[GetReplacementPage()] page_id %d not found in page_table_", (*page_ptr)->GetPageId());
    }
    return true;
  }  // end if
  return false;
}  // end GetReplacementPage

}  // namespace bustub
