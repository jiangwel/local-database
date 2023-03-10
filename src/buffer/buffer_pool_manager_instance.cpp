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
#include "common/macros.h"
#include "common/logger.h"

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
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  Page* temp_page_ptr = nullptr;
  Page** page_ptr = &temp_page_ptr;
  std::shared_ptr<frame_id_t> frame_id_ptr = std::make_shared<frame_id_t>();
  if(free_list_.empty()){
    if(GetReplacementPage(frame_id_ptr.get(),page_ptr)){
      (*page_ptr)->page_id_ = next_page_id_++;
      *page_id = (*page_ptr)->GetPageId();
    } else { return nullptr; }
  } else {
    *frame_id_ptr = free_list_.front();
    free_list_.pop_front();
    *page_id = AllocatePage();
    *page_ptr = GetPageFromePageId(*page_id);
  }// end else
  page_table_->Insert(*page_id,*frame_id_ptr);
  ResetPage(*page_ptr,*frame_id_ptr);
  (*page_ptr)->pin_count_++;
  return *page_ptr;
}// end NewPgImp

auto BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) -> Page * {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  Page* temp_ptr = nullptr;
  Page** page_ptr_ptr = &temp_ptr;
  frame_id_t frame_id = 0;
  page_table_->Find(page_id, frame_id);
  for(size_t i=0;i<pool_size_;++i){
    if(pages_[i].page_id_ == page_id){
      *page_ptr_ptr = &pages_[i];
      replacer_->SetEvictable(frame_id, false);
      replacer_->RecordAccess(frame_id);
      break;
    }// end if
  }// end for
  if(*page_ptr_ptr == nullptr){
    if(free_list_.empty()){
      if(!GetReplacementPage(&frame_id,page_ptr_ptr)){return nullptr;}
    } else {
      frame_id = free_list_.front();
      free_list_.pop_front();
      *page_ptr_ptr = GetPageFromePageId(INVALID_PAGE_ID);
    }// end else
    ResetPage(*page_ptr_ptr,frame_id);
    (*page_ptr_ptr)->page_id_ = page_id;
    page_table_->Insert(page_id,frame_id);
    char* new_page_data = new char[BUSTUB_PAGE_SIZE];
    disk_manager_->ReadPage(page_id, new_page_data);
    memcpy((*page_ptr_ptr)->GetData(), new_page_data, BUSTUB_PAGE_SIZE);
    delete[] new_page_data;
  }// end if
  (*page_ptr_ptr)->pin_count_++;
  return *page_ptr_ptr;
}// end FetchPgImp

auto BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  auto page_ptr = GetPageFromePageId(page_id);
  if(page_ptr == nullptr){return false;}
  page_ptr->pin_count_--;
  frame_id_t frame_id;
  page_table_->Find(page_id, frame_id);
  if(page_ptr->pin_count_ == 0){replacer_->SetEvictable(frame_id, true);}
  if(is_dirty){page_ptr->is_dirty_ = true;}
  return true;
}// end UnpinPgImp

auto BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  frame_id_t frame_id;
  if(page_table_->Find(page_id, frame_id)){
    auto page = GetPageFromePageId(page_id);
    disk_manager_->WritePage(page_id, page->GetData());
    page->is_dirty_ = false;
  }// end if
  return 0;
}// end FlushPgImp

void BufferPoolManagerInstance::FlushAllPgsImp() {
  for(size_t i=0;i<pool_size_;++i){
    if(pages_[i].page_id_ != INVALID_PAGE_ID){FlushPgImp(pages_[i].page_id_);}
  }// end for
}// end FlushAllPgsImp

auto BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  auto page_ptr = GetPageFromePageId(page_id);
  if(page_ptr == nullptr){return 1;}
  if(page_ptr->pin_count_ > 0){return 0;}
  frame_id_t framd_id;
  page_table_->Find(page_id, framd_id);
  DeallocatePage(page_id);
  page_ptr->is_dirty_ = 0;
  page_ptr->ResetMemory();
  page_table_->Remove(page_id);
  replacer_->Remove(framd_id);
  free_list_.push_back(framd_id);
  return 1;
}// end DeletePgImp

auto BufferPoolManagerInstance::AllocatePage() -> page_id_t {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  auto page_ptr = GetPageFromePageId(INVALID_PAGE_ID);
  page_ptr->page_id_ = next_page_id_;
  return next_page_id_++;
}// end AllocatePage

void BufferPoolManagerInstance::ResetPage(Page *page,frame_id_t frame_id){
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  page->pin_count_ = 0;
  page->is_dirty_ = false;
  replacer_->RecordAccess(frame_id);
  replacer_->SetEvictable(frame_id, 0);
}// end ResetPage

auto BufferPoolManagerInstance::GetPageFromFrameId(frame_id_t frame_id) -> Page*{
  frame_id_t temp_frame_id=0;
  Page* page_ptr = nullptr;
  for(size_t i=0;i<pool_size_;++i){
    page_table_->Find(pages_[i].page_id_, temp_frame_id);
    if(temp_frame_id==frame_id){
      page_ptr = &pages_[i];
      break;
    }// end if
  }// end for
  return page_ptr;
}// end GetPageFromFrameId

auto BufferPoolManagerInstance::GetReplacementPage(frame_id_t* frame_id_ptr,Page** page_ptr) -> bool{
  if(replacer_->Evict(frame_id_ptr)){
    *page_ptr = GetPageFromFrameId(*frame_id_ptr);
    if((*page_ptr)->IsDirty()){ disk_manager_->WritePage((*page_ptr)->GetPageId(), (*page_ptr)->GetData()); }
    page_table_->Remove((*page_ptr)->GetPageId());
    return 1;
  }// end if
  return 0; 
}// end GetReplacementPage

auto BufferPoolManagerInstance::GetPageFromePageId(page_id_t page_id) -> Page*{
  for(size_t i=0;i<pool_size_;++i){
    if(pages_[i].page_id_ == page_id){return &pages_[i];}
  }// end for
  return nullptr;
}// end GetPageFromePageId
}  // namespace bustub
