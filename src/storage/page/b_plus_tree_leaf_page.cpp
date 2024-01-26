//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_leaf_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <sstream>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

/*****************************************************************************
 * HELPER METHODS AND UTILITIES
 *****************************************************************************/

/**
 * Init method after creating a new leaf page
 * Including set page type, set current size to zero, set page id/parent_page id, set
 * next page id and set max size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_page_id, int max_size) {
  SetPageId(page_id);
  SetParentPageId(parent_page_id);
  SetPageType(IndexPageType::LEAF_PAGE);
  SetMaxSize(max_size - (max_size == LEAF_PAGE_SIZE ? 1 : 0));
  SetSize(0);
  SetLSN(INVALID_LSN);
  next_page_id_ = INVALID_PAGE_ID;
}

/**
 * Helper methods to set/get next page id
 */
INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::GetNextPageId() const -> page_id_t { return next_page_id_; }

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_LEAF_PAGE_TYPE::SetNextPageId(page_id_t next_page_id) { next_page_id_ = next_page_id; }

/*
 * Helper method to find and return the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::KeyAt(int index) const -> KeyType {
  if (index < 0 || index > this->GetSize() - 1) {
    LOG_DEBUG("KeyAt: index %d out of range %d page: %d", index, this->GetSize() - 1, this->GetPageId());
  }
  return array_[index].first;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::SetPairAt(int index, const MappingType &pair) -> bool {
  if (index < 0 || index > this->GetSize()) {
    return false;
  }
  for (int i = this->GetSize(); i > index; i--) {
    array_[i] = array_[i - 1];
  }
  array_[index] = pair;
  this->IncreaseSize(1);
  return true;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::ValueAt(int index) const -> ValueType {
  if (index < 0 || index > this->GetSize() - 1) {
    LOG_DEBUG("KeyAt: index %d out of range %d", index, this->GetSize() - 1);
  }
  return array_[index].second;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::DeletePair(const KeyType &key, KeyComparator &comparator) -> bool {
  for (int i = 0; i < this->GetSize(); i++) {
    if (comparator(array_[i].first, key) == 0) {
      for (int j = i; j < this->GetSize() - 1; j++) {
        array_[j] = array_[j + 1];
      }
      this->IncreaseSize(-1);
      return true;
    }
  }
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_LEAF_PAGE_TYPE::PairAt(int index) -> MappingType & {
  if (index < 0 || index > this->GetSize() - 1) {
    LOG_DEBUG("KeyAt: index %d out of range %d", index, this->GetSize() - 1);
  }
  return array_[index];
}

template class BPlusTreeLeafPage<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTreeLeafPage<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTreeLeafPage<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTreeLeafPage<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTreeLeafPage<GenericKey<64>, RID, GenericComparator<64>>;
}  // namespace bustub
