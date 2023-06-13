/**
 * lru_k_replacer_test.cpp
 */

#include "buffer/lru_k_replacer.h"

#include <algorithm>
#include <cstdio>
#include <memory>
#include <random>
#include <set>
#include <thread>  // NOLINT
#include <vector>

#include "gtest/gtest.h"

namespace bustub {

// void PrintAllNode(LRUKReplacer &lru_replacer){
//   auto nodes = lru_replacer.GetNodeStorePtr();
//   //遍历nodes
//   for (auto &node : *nodes) {
//     std::cout << "fid: " << node.first << " is_evictable: " << node.second->GetIsEvictable() << " timestamp_num: " <<
//     node.second->GetTimestampNum() << " k: " << node.second->GetK() << std::endl; auto history_ptr =
//     node.second->GetHistoryPtr(); for (auto &timestamp : *history_ptr) {
//       std::cout << timestamp << " ";
//     }
//     std::cout << std::endl;
//   }

// }
TEST(LRUKReplacerTest, SampleTest) {
  LRUKReplacer lru_replacer(7, 2);

  // Scenario: add six elements to the replacer. We have [1,2,3,4,5]. Frame 6 is non-evictable.
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(2);
  lru_replacer.RecordAccess(3);
  lru_replacer.RecordAccess(4);
  lru_replacer.RecordAccess(5);
  lru_replacer.RecordAccess(6);
  lru_replacer.SetEvictable(1, true);
  lru_replacer.SetEvictable(2, true);
  lru_replacer.SetEvictable(3, true);
  lru_replacer.SetEvictable(4, true);
  lru_replacer.SetEvictable(5, true);
  lru_replacer.SetEvictable(6, false);
  ASSERT_EQ(5, lru_replacer.Size());

  // Scenario: Insert access history for frame 1. Now frame 1 has two access histories.
  // All other frames have max backward k-dist. The order of eviction is [2,3,4,5,1].
  lru_replacer.RecordAccess(1);

  // Scenario: Evict three pages from the replacer. Elements with max k-distance should be popped
  // first based on LRU.
  int value;
  lru_replacer.Evict(&value);
  ASSERT_EQ(2, value);
  lru_replacer.Evict(&value);
  ASSERT_EQ(3, value);
  lru_replacer.Evict(&value);
  ASSERT_EQ(4, value);
  ASSERT_EQ(2, lru_replacer.Size());

  // Scenario: Now replacer has frames [5,1].
  // Insert new frames 3, 4, and update access history for 5. We should end with [3,1,5,4]
  lru_replacer.RecordAccess(3);
  lru_replacer.RecordAccess(4);
  lru_replacer.RecordAccess(5);
  lru_replacer.RecordAccess(4);
  lru_replacer.SetEvictable(3, true);
  lru_replacer.SetEvictable(4, true);
  ASSERT_EQ(4, lru_replacer.Size());

  // Scenario: continue looking for victims. We expect 3 to be evicted next.
  lru_replacer.Evict(&value);
  ASSERT_EQ(3, value);
  ASSERT_EQ(3, lru_replacer.Size());

  // Set 6 to be evictable. 6 Should be evicted next since it has max backward k-dist.
  lru_replacer.SetEvictable(6, true);
  ASSERT_EQ(4, lru_replacer.Size());
  lru_replacer.Evict(&value);
  ASSERT_EQ(6, value);
  ASSERT_EQ(3, lru_replacer.Size());

  // Now we have [1,5,4]. Continue looking for victims.
  lru_replacer.SetEvictable(1, false);
  ASSERT_EQ(2, lru_replacer.Size());
  ASSERT_EQ(true, lru_replacer.Evict(&value));
  ASSERT_EQ(5, value);
  ASSERT_EQ(1, lru_replacer.Size());

  // Update access history for 1. Now we have [4,1]. Next victim is 4.
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(1);
  lru_replacer.SetEvictable(1, true);
  ASSERT_EQ(2, lru_replacer.Size());
  ASSERT_EQ(true, lru_replacer.Evict(&value));
  ASSERT_EQ(value, 4);  // 说是1，但是实际上是4

  ASSERT_EQ(1, lru_replacer.Size());
  lru_replacer.Evict(&value);
  ASSERT_EQ(value, 1);
  ASSERT_EQ(0, lru_replacer.Size());

  // These operations should not modify size
  ASSERT_EQ(false, lru_replacer.Evict(&value));
  ASSERT_EQ(0, lru_replacer.Size());
  lru_replacer.Remove(1);
  ASSERT_EQ(0, lru_replacer.Size());
}

TEST(LRUKReplacerTest, EvictTest1) {
  LRUKReplacer lru_replacer(7, 3);
  int value;

  lru_replacer.RecordAccess(2);
  lru_replacer.SetEvictable(2, false);
  lru_replacer.SetEvictable(2, true);
  lru_replacer.Evict(&value);
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(2);
  lru_replacer.RecordAccess(1);
  lru_replacer.SetEvictable(2, true);
  lru_replacer.SetEvictable(1, true);
  lru_replacer.Evict(&value);

  ASSERT_EQ(true, lru_replacer.Evict(&value));
}
TEST(LRUKReplacerTest, EvictTest2) {
  LRUKReplacer lru_replacer(7, 3);
  int value;

  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(2);  // 2
  lru_replacer.RecordAccess(3);
  lru_replacer.RecordAccess(4);  // 4
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(2);  // 2
  lru_replacer.RecordAccess(3);
  lru_replacer.RecordAccess(1);
  lru_replacer.RecordAccess(2);  // 2
  lru_replacer.SetEvictable(1, true);
  lru_replacer.SetEvictable(2, true);
  lru_replacer.SetEvictable(3, true);
  lru_replacer.SetEvictable(4, true);
  lru_replacer.Evict(&value);
  ASSERT_EQ(3, value);
  lru_replacer.RecordAccess(4);  // 4
  lru_replacer.RecordAccess(4);  // 4
  lru_replacer.Evict(&value);
  ASSERT_EQ(1, value);
  lru_replacer.Evict(&value);
  ASSERT_EQ(2, value);
}
TEST(LRUKReplacerTest, ConcurrencyTest) {
  LRUKReplacer lru_replacer(1000, 3);

  lru_replacer.RecordAccess(100);
  lru_replacer.SetEvictable(100, true);
  lru_replacer.RecordAccess(101);
  lru_replacer.SetEvictable(101, true);
  lru_replacer.RecordAccess(102);
  lru_replacer.SetEvictable(102, true);
  lru_replacer.RecordAccess(103);
  lru_replacer.SetEvictable(103, true);
  lru_replacer.RecordAccess(104);
  lru_replacer.SetEvictable(104, true);
  lru_replacer.RecordAccess(105);
  lru_replacer.SetEvictable(105, true);
  lru_replacer.RecordAccess(106);
  lru_replacer.SetEvictable(106, true);
  lru_replacer.RecordAccess(107);
  lru_replacer.SetEvictable(107, true);
  lru_replacer.RecordAccess(108);
  lru_replacer.SetEvictable(108, true);
  lru_replacer.RecordAccess(109);

  for (int i = 0; i < 100; i++) {
    lru_replacer.RecordAccess(i);
    lru_replacer.SetEvictable(i, true);
  }
  lru_replacer.SetEvictable(109, true);
  for (int i = 110; i <= 121; i++) {
    lru_replacer.RecordAccess(i);
    lru_replacer.SetEvictable(i, true);
  }
  lru_replacer.RecordAccess(122);
  for (int i = 446; i <= 499; i++) {
    lru_replacer.Remove(i);
  }
  for (int i = 400; i <= 499; i++) {
    lru_replacer.Remove(i);
  }
  for (int i = 250; i <= 499; i++) {
    lru_replacer.Remove(i);
  }
  lru_replacer.Remove(0);
}
TEST(LRUKReplacerTest, DISABLE_EvictTest3) {
  LRUKReplacer lru_replacer(1000, 3);
  int value;
  // add 0-82
  for (int i = 0; i <= 250; i++) {
    lru_replacer.RecordAccess(i);
    lru_replacer.SetEvictable(i, true);
  }
  lru_replacer.RecordAccess(82);
  for (int i = 374; i <= 499; i++) {
    lru_replacer.SetEvictable(i, 0);
  }
  // remove 0-99
  for (int i = 0; i <= 99; i++) {
    lru_replacer.Remove(i);
  }
  for (int i = 100; i <= 249; i++) {
    ASSERT_EQ(1, lru_replacer.Evict(&value));
  }
  ASSERT_EQ(1, lru_replacer.Evict(&value));
}
}  // namespace bustub
