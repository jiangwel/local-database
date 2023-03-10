//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance_test.cpp
//
// Identification: test/buffer/buffer_pool_manager_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager_instance.h"

#include <cstdio>
#include <random>
#include <string>

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"

namespace bustub {
// Check whether pages containing terminal characters can be recovered
// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, BinaryDataTest) {
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;

  std::random_device r;
  std::default_random_engine rng(r());
  std::uniform_int_distribution<char> uniform_dist(0);

  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);

  page_id_t page_id_temp;
  auto *page0 = bpm->NewPage(&page_id_temp);

  // Scenario: The buffer pool is empty. We should be able to create a new page.
  ASSERT_NE(nullptr, page0);
  EXPECT_EQ(0, page_id_temp);

  char random_binary_data[BUSTUB_PAGE_SIZE];
  // Generate random binary data
  for (char &i : random_binary_data) {
    i = uniform_dist(rng);
  }

  // Insert terminal characters both in the middle and at end
  random_binary_data[BUSTUB_PAGE_SIZE / 2] = '\0';
  random_binary_data[BUSTUB_PAGE_SIZE - 1] = '\0';

  // Scenario: Once we have a page, we should be able to read and write content.
  std::memcpy(page0->GetData(), random_binary_data, BUSTUB_PAGE_SIZE);
  EXPECT_EQ(0, std::memcmp(page0->GetData(), random_binary_data, BUSTUB_PAGE_SIZE));

  // Scenario: We should be able to create new pages until we fill up the buffer pool.
  // page_id 1-9
  for (size_t i = 1; i < buffer_pool_size; ++i) {
    EXPECT_NE(nullptr, bpm->NewPage(&page_id_temp));
  }

  // Scenario: Once the buffer pool is full, we should not be able to create any new pages.
  for (size_t i = buffer_pool_size; i < buffer_pool_size * 2; ++i) {
    EXPECT_EQ(nullptr, bpm->NewPage(&page_id_temp));
  }

  // Scenario: After unpinning pages {0, 1, 2, 3, 4} we should be able to create 5 new pages
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(true, bpm->UnpinPage(i, true));
    bpm->FlushPage(i);
  }
  // page_id 10-14
  for (int i = 0; i < 5; ++i) {
    EXPECT_NE(nullptr, bpm->NewPage(&page_id_temp));
    bpm->UnpinPage(page_id_temp, false);
  }
  // Scenario: We should be able to fetch the data we wrote a while ago.
  page0 = bpm->FetchPage(0);
  EXPECT_EQ(0, memcmp(page0->GetData(), random_binary_data, BUSTUB_PAGE_SIZE));
  EXPECT_EQ(true, bpm->UnpinPage(0, true));

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}

// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, SampleTest) {
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;

  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);
  // init code 
  page_id_t page_id_temp;
  // page0 pin cont = 1
  auto *page0 = bpm->NewPage(&page_id_temp);

  // Scenario: The buffer pool is empty. We should be able to create a new page.
  ASSERT_NE(nullptr, page0);
  EXPECT_EQ(0, page_id_temp);

  // Scenario: Once we have a page, we should be able to read and write content.
  // wite data to page0
  snprintf(page0->GetData(), BUSTUB_PAGE_SIZE, "Hello");
  EXPECT_EQ(0, strcmp(page0->GetData(), "Hello"));

  // Scenario: We should be able to create new pages until we fill up the buffer pool.
  // page_id 1-9
  for (size_t i = 1; i < buffer_pool_size; ++i) {
    EXPECT_NE(nullptr, bpm->NewPage(&page_id_temp));
  }

  // Scenario: Once the buffer pool is full, we should not be able to create any new pages.
  for (size_t i = buffer_pool_size; i < buffer_pool_size * 2; ++i) {
    EXPECT_EQ(nullptr, bpm->NewPage(&page_id_temp));
  }

  // Scenario: After unpinning pages {0, 1, 2, 3, 4} and pinning another 4 new pages,
  // there would still be one buffer page left for reading page 0.
  // page id 0-4 pin cont = 0
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(true, bpm->UnpinPage(i, true));
  }

  // page_id 10-13 be allocated,page 0 be writted to disk
  for (int i = 0; i < 4; ++i) {
    EXPECT_NE(nullptr, bpm->NewPage(&page_id_temp));
  }

  //bug
  // Scenario: We should be able to fetch the data we wrote a while ago.
  // page0 ,get from disk
  page0 = bpm->FetchPage(0);
  EXPECT_EQ(0, strcmp(page0->GetData(), "Hello"));

  // Scenario: If we unpin page 0 and then make a new page, all the buffer pages should
  // now be pinned. Fetching page 0 should fail.
  EXPECT_EQ(true, bpm->UnpinPage(0, true));
  // page_id 14,evict page 0 then crate page 14
  EXPECT_NE(nullptr, bpm->NewPage(&page_id_temp));
  // can't fetch page0,because no more space
  EXPECT_EQ(nullptr, bpm->FetchPage(0));
  //bug

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}

// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, UnitTestNewPgImp){
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;
  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);
  //
  for(int i=0;i<10;i++){
    page_id_t page_id_temp;
    bpm->NewPage(&page_id_temp);
  }
  //test free_list_ is empty
  //all page id pinned can't create new page
  page_id_t page_id_temp;
  auto* page = bpm->NewPage(&page_id_temp);
  EXPECT_EQ(nullptr, page);
  //evict page,then create new page
  bpm->UnpinPage(0, true);
  auto* page1 = bpm->NewPage(&page_id_temp);
  EXPECT_NE(nullptr, page1);
  EXPECT_EQ(10, page_id_temp);

  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}

// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, DISABLED_UnitTestUnpinPgImp){
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;

  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);
// no page id reutrn false
  EXPECT_EQ(false,bpm->UnpinPage(0, true));
// exzit page id
  page_id_t page_id_temp;
  bpm->NewPage(&page_id_temp);
// pin cont is 1 ,be evictble
// return true
  EXPECT_EQ(true,bpm->UnpinPage(0, true));

  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}

// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, UnitTestFetchPgImp){
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;

  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);
//
  for(int i=0;i<10;i++){
    page_id_t page_id_temp;
    bpm->NewPage(&page_id_temp);
  }
  auto page1 = bpm->FetchPage(0);
  EXPECT_NE(nullptr, page1);
  auto page2 = bpm->FetchPage(10);
  EXPECT_EQ(nullptr, page2);
  bpm->UnpinPage(1, false);
  bpm->UnpinPage(2, false);
  //auto page3 = bpm->FetchPage(101);
  //EXPECT_NE(nullptr, page3);

//
  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}

// NOLINTNEXTLINE
TEST(BufferPoolManagerInstanceTest, UnitTestDeletePgImp){
  const std::string db_name = "test.db";
  const size_t buffer_pool_size = 10;
  const size_t k = 5;

  auto *disk_manager = new DiskManager(db_name);
  auto *bpm = new BufferPoolManagerInstance(buffer_pool_size, disk_manager, k);

  EXPECT_EQ(1,bpm->DeletePage(0));
  page_id_t page_id_temp;
  bpm->NewPage(&page_id_temp);

  EXPECT_EQ(0,bpm->DeletePage(0));
  bpm->UnpinPage(0, true);
  EXPECT_EQ(1,bpm->DeletePage(0));

  disk_manager->ShutDown();
  remove("test.db");

  delete bpm;
  delete disk_manager;
}
}  // namespace bustub
