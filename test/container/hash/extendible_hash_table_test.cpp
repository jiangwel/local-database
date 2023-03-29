/**
 * extendible_hash_test.cpp
 */

#include <memory>
#include <iostream>
#include <thread>  // NOLINT

#include "container/hash/extendible_hash_table.h"
#include "gtest/gtest.h"
#include "common/logger.h"

namespace bustub {
void PrintDir(ExtendibleHashTable<int, std::string> *table) {
  for (size_t i = 0; i < table->dir_.size(); i++) {
    std::cout << "dir_[" << i << "]: ";
    auto list = table->dir_[i]->GetItems();
    for (auto it = list.begin(); it != list.end(); it++) {
      std::cout <<it->first<<" ";
    }
  std::cout << std::endl;
  }
}

TEST(ExtendibleHashTableTest, DISABLED_SampleTest) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(2);
  table->Insert(1, "a");
  table->Insert(2, "b");
  table->Insert(3, "c");
  table->Insert(4, "d");
  table->Insert(5, "e");
  table->Insert(6, "f");
  table->Insert(7, "g");
  table->Insert(8, "h");
  table->Insert(9, "i");
  EXPECT_EQ(3, table->GetGlobalDepth());
  EXPECT_EQ(2, table->GetLocalDepth(0));
  EXPECT_EQ(3, table->GetLocalDepth(1));
  EXPECT_EQ(2, table->GetLocalDepth(2));
  EXPECT_EQ(2, table->GetLocalDepth(3));

  std::string result;
  table->Find(9, result);
  EXPECT_EQ("i", result);
  table->Find(8, result);
  EXPECT_EQ("h", result);
  table->Find(2, result);
  EXPECT_EQ("b", result);
  EXPECT_FALSE(table->Find(10, result));

  EXPECT_TRUE(table->Remove(8));
  EXPECT_TRUE(table->Remove(4));
  EXPECT_TRUE(table->Remove(1));
  EXPECT_FALSE(table->Remove(20));
}

TEST(ExtendibleHashTableTest, DISABLED_ConcurrentInsertTest) {
  const int num_runs = 50;
  const int num_threads = 3;

  // Run concurrent test multiple times to guarantee correctness.
  for (int run = 0; run < num_runs; run++) {
    auto table = std::make_unique<ExtendibleHashTable<int, int>>(2);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int tid = 0; tid < num_threads; tid++) {
      threads.emplace_back([tid, &table]() { table->Insert(tid, tid); });
    }
    for (int i = 0; i < num_threads; i++) {
      threads[i].join();
    }

    EXPECT_EQ(table->GetGlobalDepth(), 1);
    for (int i = 0; i < num_threads; i++) {
      int val;
      EXPECT_TRUE(table->Find(i, val));
      EXPECT_EQ(i, val);
    }
  }
}

TEST(ExtendibleHashTableTest,DISABLED_RedistributeBucketTest){
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(2);
  table->Insert(1, "1");
  table->Insert(8, "8");
  table->Insert(3, "3");
  table->Insert(16, "16");
  table->Insert(5, "5");
  EXPECT_EQ(2, table->GetGlobalDepth());
  table->Insert(12, "12");
  EXPECT_EQ(3, table->GetGlobalDepth());
  std::string result;
  EXPECT_EQ(true, table->Find(12, result));

}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest0) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  //EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest1) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(21, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest3) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(21, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest7) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(21, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(21, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest2) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(21, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest15) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(21, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, RemoveTest5) {
  auto table = std::make_shared<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(21, "c");
  table->Insert(11, "c");
  table->Insert(19, "c");
  PrintDir(table.get());
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest11) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(21, "c");
  table->Insert(19, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}

TEST(ExtendibleHashTableTest, DISABLED_RemoveTest19) {
  auto table = std::make_unique<ExtendibleHashTable<int, std::string>>(4);
  //depth 0
  table->Insert(40, "a");
  table->Insert(12, "b");
  table->Insert(24, "c");
  table->Insert(32, "c");
  //depth 1
  table->Insert(1, "c");
  //depth 2
  table->Insert(4, "c");
  table->Insert(3, "c");
  table->Insert(7, "c");
  table->Insert(2, "c");
  //depth 3
  table->Insert(48, "c");
  table->Insert(15, "c");
  table->Insert(5, "c");
  table->Insert(11, "c");
  table->Insert(21, "c");
  std::string result;
  EXPECT_EQ(true, table->Find(4, result));
  EXPECT_EQ(true, table->Find(21, result));
}


}  // namespace bustub
