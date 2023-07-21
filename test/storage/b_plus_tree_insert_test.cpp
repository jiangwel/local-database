//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_insert_test.cpp
//
// Identification: test/storage/b_plus_tree_insert_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cstdio>

#include "buffer/buffer_pool_manager_instance.h"
#include "gtest/gtest.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT

namespace bustub {

TEST(BPlusTreeTests, InsertTest1) {
  // // create KeyComparator and index schema
  // auto key_schema = ParseCreateStatement("a bigint");
  // GenericComparator<8> comparator(key_schema.get());

  // auto *disk_manager = new DiskManager("test.db");
  // BufferPoolManager *bpm = new BufferPoolManagerInstance(50, disk_manager);
  // // create b+ tree
  // BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator, 2, 3);
  // GenericKey<8> index_key;
  // RID rid;
  // // create transaction
  // auto *transaction = new Transaction(0);

  // // create and fetch header_page
  // page_id_t page_id;
  // auto header_page = bpm->NewPage(&page_id);
  // ASSERT_EQ(page_id, HEADER_PAGE_ID);
  // (void)header_page;

  // // 插入第一个kv pair
  // int64_t key = 42;
  // int64_t value = key & 0xFFFFFFFF;
  // rid.Set(static_cast<int32_t>(key), value);
  // index_key.SetFromInteger(key);
  // tree.Insert(index_key, rid, transaction);
  // // 捕获root
  // auto root_page_id = tree.GetRootPageId();
  // std::cout << "root page id is: " << root_page_id << std::endl;
  // auto root_page = reinterpret_cast<BPlusTreePage *>(bpm->FetchPage(root_page_id)->GetData());
  // // 又把root_page转换成leaf_page,检查leaf_page的size和key是否正确
  // auto root_as_leaf = reinterpret_cast<BPlusTreeLeafPage<GenericKey<8>, RID, GenericComparator<8>> *>(root_page);
  // ASSERT_EQ(root_as_leaf->GetSize(), 1);
  // ASSERT_EQ(comparator(root_as_leaf->KeyAt(0), index_key), 0);
  // // 检查root不是空的并且等于leaf
  // ASSERT_NE(root_page, nullptr);
  // ASSERT_TRUE(root_page->IsLeafPage());

  // bpm->UnpinPage(root_page_id, false);
  // bpm->UnpinPage(HEADER_PAGE_ID, true);
  // delete transaction;
  // delete disk_manager;
  // delete bpm;
  // remove("test.db");
  // remove("test.log");

    // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManager("test.db");
  BufferPoolManager *bpm = new BufferPoolManagerInstance(50, disk_manager);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator,2,3);
  GenericKey<8> index_key;
  RID rid;
  // create transaction
  auto *transaction = new Transaction(0);

  // create and fetch header_page
  page_id_t page_id;
  auto header_page = bpm->NewPage(&page_id);
  ASSERT_EQ(page_id, HEADER_PAGE_ID);
  (void)header_page;

  // 插入5个kv pair
  std::vector<int64_t> keys;
  for(int i=0;i<7;++i){
    int64_t value = i & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 1), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid, transaction);
    keys.push_back(i);
  }

  // 检查
  std::vector<RID> rids;
  for (auto key : keys) {
    std::cout<<"check key: "<<key<<std::endl;
    rids.clear();
    index_key.SetFromInteger(key);
    
    EXPECT_EQ(tree.GetValue(index_key, &rids),1);
    if(!rids.size()){
      std::cout<<"key: "<<key<<" not found"<<std::endl;
    }
    EXPECT_EQ(rids.size(), 1);


    // int64_t value = key & 0xFFFFFFFF;
    // EXPECT_EQ(rids[0].GetSlotNum(), value);
  }

  bpm->UnpinPage(HEADER_PAGE_ID, true);
  delete transaction;
  delete disk_manager;
  delete bpm;
  remove("test.db");
  remove("test.log");
}

TEST(BPlusTreeTests, DISABLED_InsertTest2) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManager("test.db");
  BufferPoolManager *bpm = new BufferPoolManagerInstance(50, disk_manager);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator, 2, 3);
  GenericKey<8> index_key;
  RID rid;
  // create transaction
  auto *transaction = new Transaction(0);

  // create and fetch header_page
  page_id_t page_id;
  auto header_page = bpm->NewPage(&page_id);
  (void)header_page;
  // 插入5个kv pair
  std::vector<int64_t> keys = {1, 2, 3, 4, 5};
  for (auto key : keys) {
    std::cout << std::endl << "insert key: " << key << std::endl;
    int64_t value = key & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(key >> 32), value);
    index_key.SetFromInteger(key);
    tree.Insert(index_key, rid, transaction);
  }
  // std::cout << std::endl << "insert end" << std::endl;
  // 捕获5个value,检查对不对
  std::vector<RID> rids;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    tree.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
  // std::cout << std::endl << "cheak 1 end" << std::endl;
  int64_t size = 0;
  bool is_present;

  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    is_present = tree.GetValue(index_key, &rids);

    EXPECT_EQ(is_present, true);
    EXPECT_EQ(rids.size(), 1);
    EXPECT_EQ(rids[0].GetPageId(), 0);
    EXPECT_EQ(rids[0].GetSlotNum(), key);
    size = size + 1;
  }
  std::cout << std::endl << "cheak 2 end" << std::endl;

  EXPECT_EQ(size, keys.size());

  bpm->UnpinPage(HEADER_PAGE_ID, true);
  delete transaction;
  delete disk_manager;
  delete bpm;
  remove("test.db");
  remove("test.log");
}

TEST(BPlusTreeTests, DISABLED_InsertTest3) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManager("test.db");
  BufferPoolManager *bpm = new BufferPoolManagerInstance(50, disk_manager);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator);
  GenericKey<8> index_key;
  RID rid;
  // create transaction
  auto *transaction = new Transaction(0);

  // create and fetch header_page
  page_id_t page_id;
  auto header_page = bpm->NewPage(&page_id);
  ASSERT_EQ(page_id, HEADER_PAGE_ID);
  (void)header_page;
  // 插入
  std::vector<int64_t> keys = {5, 4, 3, 2, 1};
  for (auto key : keys) {
    int64_t value = key & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(key >> 32), value);
    index_key.SetFromInteger(key);
    tree.Insert(index_key, rid, transaction);
  }
  // 检查
  std::vector<RID> rids;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    tree.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }

  int64_t start_key = 1;
  int64_t current_key = start_key;
  index_key.SetFromInteger(start_key);
  for (auto iterator = tree.Begin(index_key); iterator != tree.End(); ++iterator) {
    auto location = (*iterator).second;
    EXPECT_EQ(location.GetPageId(), 0);
    EXPECT_EQ(location.GetSlotNum(), current_key);
    current_key = current_key + 1;
  }

  EXPECT_EQ(current_key, keys.size() + 1);

  start_key = 3;
  current_key = start_key;
  index_key.SetFromInteger(start_key);
  for (auto iterator = tree.Begin(index_key); iterator != tree.End(); ++iterator) {
    auto location = (*iterator).second;
    EXPECT_EQ(location.GetPageId(), 0);
    EXPECT_EQ(location.GetSlotNum(), current_key);
    current_key = current_key + 1;
  }

  bpm->UnpinPage(HEADER_PAGE_ID, true);
  delete transaction;
  delete disk_manager;
  delete bpm;
  remove("test.db");
  remove("test.log");
}
}  // namespace bustub