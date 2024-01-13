//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/include/index/b_plus_tree.h
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#pragma once

#include <queue>
#include <string>
#include <vector>

#include "concurrency/transaction.h"
#include "storage/index/index_iterator.h"
#include "storage/page/b_plus_tree_internal_page.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType, KeyComparator>
#define INVALID_THREAD_ID std::thread::id()

enum class OperateType { Other = 0, Find, Insert, Delete, Iterator };
/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain actual data.
 * (1) We only support unique key
 * (2) support insert & remove
 * (3) The structure should shrink and grow dynamically
 * (4) Implement index iterator for range scan
 */
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
  using InternalPage = BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>;
  using LeafPage = BPlusTreeLeafPage<KeyType, ValueType, KeyComparator>;

 public:
  explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                     int leaf_max_size = LEAF_PAGE_SIZE, int internal_max_size = INTERNAL_PAGE_SIZE);

  // Returns true if this B+ tree has no keys and values.
  auto IsEmpty() const -> bool;

  // Insert a key-value pair into this B+ tree.
  auto Insert(const KeyType &key, const ValueType &value, Transaction *transaction = nullptr) -> bool;

  // Remove a key and its value from this B+ tree.
  void Remove(const KeyType &key, Transaction *transaction = nullptr);

  // return the value associated with a given key
  auto GetValue(const KeyType &key, std::vector<ValueType> *result, Transaction *transaction = nullptr) -> bool;

  // return the page id of the root node
  auto GetRootPageId() -> page_id_t;

  // index iterator
  auto Begin() -> INDEXITERATOR_TYPE;
  auto Begin(const KeyType &key) -> INDEXITERATOR_TYPE;
  auto End() -> INDEXITERATOR_TYPE;

  // print the B+ tree
  void Print(BufferPoolManager *buffer_pool_manager_);

  // draw the B+ tree
  void Draw(BufferPoolManager *buffer_pool_manager_, const std::string &outf);

  // read data from file and insert one by one
  void InsertFromFile(const std::string &file_name, Transaction *transaction = nullptr);

  // read data from file and remove one by one
  void RemoveFromFile(const std::string &file_name, Transaction *transaction = nullptr);

 private:
  void UpdateRootPageId(int insert_record = 0);

  /* Debug Routines for FREE!! */
  void ToGraph(BPlusTreePage *page, BufferPoolManager *buffer_pool_manager_, std::ofstream &out) const;

  void ToString(BPlusTreePage *page, BufferPoolManager *buffer_pool_manager_) const;

  void InsertNode(BPlusTreePage *node, const KeyType &key, const ValueType &value);
  auto GetLeaf(const KeyType &key, OperateType operator_type, Transaction *transaction = nullptr,
               Page *node_page = nullptr) -> std::tuple<LeafPage *, int>;
  void InsertParent(BPlusTreePage *page1, BPlusTreePage *page2, const KeyType &key, const ValueType &value,
                    Transaction *transaction = nullptr);
  void RemoveEntry(BPlusTreePage *bptree_page, const KeyType &key, Transaction *transaction = nullptr);
  void LockAndUnlock(Page *page, BPlusTreePage *node, OperateType operator_type, Transaction *transaction = nullptr);
  void ReleaseResourcesd(Transaction *transaction = nullptr);
  void MakeRoot(const KeyType &key, const ValueType &value);
  void InsertInFillNode(LeafPage *leaf1,const KeyType &key, const ValueType &value,Transaction *transaction);
  void RenewRoot(BPlusTreePage *page1, BPlusTreePage *page2,const KeyType &key);
  void InsertInFillParent(BPlusTreePage *page1, BPlusTreePage *page2,InternalPage * parent,const KeyType &key, const ValueType &value, Transaction *transaction);
  void ReplaceRootByChildren(InternalPage *old_root);
  auto GetSiblingIdx(InternalPage *parent_page,const int page_id)->int;
  void Coalesce(bool is_sibling_brother,BPlusTreePage *bptree_page,BPlusTreePage *sibling_page);
  // member variable
  std::string index_name_;
  BufferPoolManager *buffer_pool_manager_;
  KeyComparator comparator_;
  int leaf_max_size_;
  int internal_max_size_;
  page_id_t root_page_id_;
  // RootPageIDLatch* root_page_id_latch_;
  // Page virtual_page_{};
  // Page *virtual_page_ptr_;
  ReaderWriterLatch root_page_id_latch_;
};

}  // namespace bustub