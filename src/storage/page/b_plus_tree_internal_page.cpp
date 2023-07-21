//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_internal_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <sstream>

#include "common/exception.h"
#include "common/logger.h"
#include "storage/page/b_plus_tree_internal_page.h"

namespace bustub {
/*****************************************************************************
 * HELPER METHODS AND UTILITIES
 *****************************************************************************/
/*
 * Init method after creating a new internal page
 * Including set page type, set current size, set page id, set parent id and set
 * max page size
 */
INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::Init(page_id_t page_id, page_id_t parent_id, int max_size) {
  SetPageId(page_id);
  SetParentPageId(parent_id);
  SetPageType(IndexPageType::INTERNAL_PAGE);
  SetMaxSize(max_size);
  SetSize(0);
  SetLSN(INVALID_LSN);
}
/*
 * Helper method to get/set the key associated with input "index"(a.k.a
 * array offset)
 */
INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_INTERNAL_PAGE_TYPE::KeyAt(int index) const -> KeyType {
  if (index < 0 || index > this->GetSize() - 1) {
    LOG_DEBUG("KeyAt: index %d out of range %d", index, this->GetSize() - 1);
  }
  // key where index 0 is invild
  return array_[index + 1].first;
}

INDEX_TEMPLATE_ARGUMENTS
void B_PLUS_TREE_INTERNAL_PAGE_TYPE::SetKeyAt(int index, const KeyType &key) {
  if (index < 0 || index > this->GetSize()) {
    LOG_DEBUG("SetKeyAt: index %d out of range %d", index, this->GetSize());
  }
  array_[index + 1].first = key;
}

/*
 * Helper method to get the value associated with input "index"(a.k.a array
 * offset)
 */
INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_INTERNAL_PAGE_TYPE::ValueAt(int index) const -> ValueType {
  if (index < 0 || index > this->GetSize() - 1) {
    LOG_DEBUG("ValueAt: index %d out of range %d", index, this->GetSize() - 1);
  }
  return array_[index].second;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_INTERNAL_PAGE_TYPE::SetPairAt(int index, const MappingType &pair) -> bool {
  if (index < 0 || index > this->GetSize()) {
    return false;
  }
  if (index == this->GetSize()) {
    array_[index] = pair;
    return true;
  }
  for (int i = this->GetSize(); i > index; i--) {
    array_[i] = array_[i - 1];
  }
  array_[index] = pair;
  return true;
}

INDEX_TEMPLATE_ARGUMENTS
auto B_PLUS_TREE_INTERNAL_PAGE_TYPE::DeletePair(const KeyType &key, KeyComparator &comparator) -> bool {
  // array_[0] is invalid key , pointer
  for (int i = 1; i < this->GetSize(); i++) {
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

// valuetype for internalNode should be page id_t
template class BPlusTreeInternalPage<GenericKey<4>, page_id_t, GenericComparator<4>>;
template class BPlusTreeInternalPage<GenericKey<8>, page_id_t, GenericComparator<8>>;
template class BPlusTreeInternalPage<GenericKey<16>, page_id_t, GenericComparator<16>>;
template class BPlusTreeInternalPage<GenericKey<32>, page_id_t, GenericComparator<32>>;
template class BPlusTreeInternalPage<GenericKey<64>, page_id_t, GenericComparator<64>>;
}  // namespace bustub
