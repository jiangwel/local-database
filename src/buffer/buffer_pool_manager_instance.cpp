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
  //LOG_INFO("$0");
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  Page* page_ptr = nullptr;
  std::shared_ptr<frame_id_t> frame_id_ptr = std::make_shared<frame_id_t>();
  if(free_list_.empty()){
    if(replacer_->Evict(frame_id_ptr.get())){
      page_table_->Find(*frame_id_ptr, *page_id);
      page_ptr=&pages_[*page_id];
      if(page_ptr->IsDirty()){
        disk_manager_->WritePage(*page_id, page_ptr->GetData());
      }
      ResetMemoryAndMetadata(page_ptr);
    } else {
      return nullptr;
    }
  } else {
  //LOG_INFO("$1");
    *frame_id_ptr = free_list_.front();
  //LOG_INFO("$10");
    free_list_.pop_front();
  //LOG_INFO("$11");
    *page_id = AllocatePage();
  //LOG_INFO("$100");
  //LOG_INFO("%d", *frame_id_ptr);
    page_table_->Insert(*frame_id_ptr,*page_id);
  //LOG_INFO("$101");
  //LOG_INFO("$110");
    page_ptr = &pages_[*page_id];
  //LOG_INFO("$111");
  }
  replacer_->RecordAccess(*frame_id_ptr);
  replacer_->SetEvictable(*frame_id_ptr, false);
  page_ptr->pin_count_++;
  return page_ptr;
}

auto BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) -> Page * { return nullptr; }

auto BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  for(auto &page : pages_){
    if(page.page_id_ == page_id){
      if(page.pin_count_ == 0){
        return false;
      }
      page.pin_count_--;
      if(page.pin_count_ == 0){
        replacer_->SetEvictable(page_id, true);
      }
      if(is_dirty){
        page.is_dirty_ = true;
      }
      return true;
    }
  }
  return false;
}

auto BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) -> bool { return false; }

void BufferPoolManagerInstance::FlushAllPgsImp() {}

auto BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) -> bool { return false; }

auto BufferPoolManagerInstance::AllocatePage() -> page_id_t {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  //遍历pages_
  page_id_t page_id = INVALID_PAGE_ID;
  for (size_t i = 0; i < pool_size_; ++i) {
    if (pages_[i].page_id_ == INVALID_PAGE_ID) {
      pages_[i].page_id_ = i;
      page_id = i;
      break;
    }
  }
  return page_id;
}

void BufferPoolManagerInstance::ResetMemoryAndMetadata(Page *page){
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  page->ResetMemory();
  page->page_id_ = INVALID_PAGE_ID;
  page->pin_count_ = 0;
  page->is_dirty_ = false;
}
}  // namespace bustub
