auto BufferPoolManagerInstance::NewPgImp(page_id_t *page_id) -> Page * {

  if (!free_list_.empty()) {

  } else {
    // assert(replacer_->Evict(&frame_id));
    replacer_->Evict(&frame_id);
    page_id_t evicted_page_id = pages_[frame_id].GetPageId();

    if (pages_[frame_id].IsDirty()) {
      disk_manager_->WritePage(evicted_page_id, pages_[frame_id].GetData());
      pages_[frame_id].is_dirty_ = false;
    }

    pages_[frame_id].ResetMemory();

    page_table_->Remove(evicted_page_id);
  }
}

auto BufferPoolManagerInstance::FetchPgImp(page_id_t page_id) -> Page * {

  if (!free_list_.empty()) {

  } else {
    // assert(replacer_->Evict(&frame_id));
    replacer_->Evict(&frame_id);
    page_id_t evicted_page_id = pages_[frame_id].GetPageId();

    if (pages_[frame_id].IsDirty()) {
      disk_manager_->WritePage(evicted_page_id, pages_[frame_id].GetData());
      pages_[frame_id].is_dirty_ = false;
    }

    pages_[frame_id].ResetMemory();

    page_table_->Remove(evicted_page_id);
  }

}

auto BufferPoolManagerInstance::UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool {
  if (pages_[frame_id].GetPinCount() <= 0) {
    return false;
  }
}

auto BufferPoolManagerInstance::FlushPgImp(page_id_t page_id) -> bool {
  if (page_id == INVALID_PAGE_ID) {
    return false;
  }
}

auto BufferPoolManagerInstance::DeletePgImp(page_id_t page_id) -> bool {

  pages_[frame_id].pin_count_ = 0;

}
