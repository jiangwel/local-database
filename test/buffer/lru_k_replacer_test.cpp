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
  TEST(LRUKReplacerTest, tt) {
    LRUKReplacer lru_replacer(7, 2);

    // Scenario: add six elements to the replacer. We have [1,2,3,4,5]. Frame 6 is non-evictable.
    lru_replacer.RecordAccess(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(3);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(4);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(5);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
    // [5,3,1]
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(3);
    // [5,3,4,1]
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(4);
    // [3,4,1,5]
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(5);
    // [3,1,5,4]
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(4);
    lru_replacer.SetEvictable(3, true);
    lru_replacer.SetEvictable(4, true);
    ASSERT_EQ(4, lru_replacer.Size());

    // Scenario: continue looking for victims. We expect 3 to be evicted next.
    lru_replacer.Evict(&value);
    ASSERT_EQ(3, value);
    ASSERT_EQ(3, lru_replacer.Size());

    // Set 6 to be evictable. 6 Should be evicted next since it has max backward k-dist.
    // [6,1,5,4]
    lru_replacer.SetEvictable(6, true);
    ASSERT_EQ(4, lru_replacer.Size());
    lru_replacer.Evict(&value);
    ASSERT_EQ(6, value);
    ASSERT_EQ(3, lru_replacer.Size());

    // Now we have [1,5,4]. Continue looking for victims.
    lru_replacer.SetEvictable(1, false);
    // [5,4]
    ASSERT_EQ(2, lru_replacer.Size());
    ASSERT_EQ(true, lru_replacer.Evict(&value));
    ASSERT_EQ(5, value);
    // [4]
    ASSERT_EQ(1, lru_replacer.Size());

    // Update access history for 1. Now we have [4,1]. Next victim is 4.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    lru_replacer.RecordAccess(1);
    lru_replacer.SetEvictable(1, true);
    ASSERT_EQ(2, lru_replacer.Size());
    // [4,1]
    ASSERT_EQ(true, lru_replacer.Evict(&value));
    ASSERT_EQ(value, 4);
    //  [1]
    ASSERT_EQ(1, lru_replacer.Size());
    lru_replacer.Evict(&value);
    ASSERT_EQ(value, 1);
    // []
    ASSERT_EQ(0, lru_replacer.Size());

    // This operation should not modify size
    ASSERT_EQ(false, lru_replacer.Evict(&value));
    ASSERT_EQ(0, lru_replacer.Size());
  }
  /*
    TEST(LRUKReplacerTest, DISABLED_SampleTest) {
      LRUKReplacer lruk_replacer(5, 5);
      for (int i = 1;i <= 5;++i) {
        lruk_replacer.RecordAccess(i);
        lruk_replacer.PrintRecord(i);
      }
      for (int i = 0;i < 20;++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        lruk_replacer.RecordAccess(2);
      }
      lruk_replacer.PrintRecord(2);
    }


    TEST(LRUKReplacerTest, DISABLED_SampleTest) {
      LRUKReplacer lruk_replacer(5, 5);
      for (int i = 1;i <= 5;++i) {
        lruk_replacer.RecordAccess(i);
      }
      //当frame id无效时，抛出异常
      ASSERT_THROW(lruk_replacer.SetEvictable(6, true), std::exception);
      //当frame id有效，且set_evictable为true时，设置frame的is_evictable属性，replacer size增加
      lruk_replacer.SetEvictable(1, true);
      ASSERT_EQ(lruk_replacer.GetNodeStorePtr()->find(1)->second->GetIsEvictable(), true);
      ASSERT_EQ(lruk_replacer.Size(), 6);
      //当frame id有效，且set_evictable为false时，设置frame的is_evictable属性，replacer size减少
      lruk_replacer.SetEvictable(1, false);
      ASSERT_EQ(lruk_replacer.GetNodeStorePtr()->find(1)->second->GetIsEvictable(), false);
      ASSERT_EQ(lruk_replacer.Size(), 5);

    }

    TEST(LRUKReplacerTest, DISABLED_SampleTest) {
      LRUKReplacer lruk_replacer(5, 5);
      for (int i = 1;i <= 5;++i) {
        lruk_replacer.RecordAccess(i);
        lruk_replacer.SetEvictable(i, true);
      }
      //当frame id有效时，且is evictable时,删除frame,并且replacer size减少
      lruk_replacer.Remove(1);
      ASSERT_EQ(lruk_replacer.Size(), 4);
      lruk_replacer.Remove(3);
      ASSERT_EQ(lruk_replacer.Size(), 3);
      //当frame id有效时，且is not evictable时,抛出异常
      lruk_replacer.SetEvictable(2, false);
      ASSERT_THROW(lruk_replacer.Remove(2), std::exception);
      //当frame id无效时，退出函数
      lruk_replacer.Remove(6);
      ASSERT_EQ(lruk_replacer.Size(), 2);
    }
    //Add implementation  for evict() that Find the frame with largest backward k-distance and evict that frame. Only
    frames that are marked as 'evictable' are candidates for eviction. A frame with less than k historical references is
    given +inf as its backward k-distance. If multiple frames have inf backward k-distance, then evict frame with earliest
    timestamp based on LRU. Successful eviction of a frame should decrement the size of replacer and remove the frame's
    access history.
    //请为Evict函数编写测试用例
    TEST(LRUKReplacerTest, DISABLED_SampleTest) {
      //测试Evict函数，当node_store_ptr_为空时或者没有is_evictable时，返回false
      LRUKReplacer lruk_replacer(5, 5);
      for (int i = 1;i <= 5;++i) {
        lruk_replacer.RecordAccess(i);
      }
      int value;
      ASSERT_EQ(false, lruk_replacer.Evict(&value));
      for (int i = 1;i <= 5;++i) {
        lruk_replacer.SetEvictable(i, true);
      }
      //当有多个frame有至少k+1个时间戳,替换backward_k_distance最大的frame,替换器size减1,frame_id为正确的帧
      for (int i = 0;i < 6;i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        lruk_replacer.RecordAccess(2);
        lruk_replacer.RecordAccess(3);
        lruk_replacer.RecordAccess(4);
      }
      for (int i = 0;i < 1;i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        lruk_replacer.RecordAccess(2);
        lruk_replacer.RecordAccess(3);
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(2000));
      lruk_replacer.RecordAccess(3);

      ASSERT_EQ(true, lruk_replacer.Evict(&value));
      ASSERT_EQ(value, 3);
      ASSERT_EQ(lruk_replacer.Size(), 4);
      ASSERT_EQ(true, lruk_replacer.Evict(&value));
      ASSERT_EQ(value, 2);
      ASSERT_EQ(lruk_replacer.Size(), 3);
      ASSERT_EQ(true, lruk_replacer.Evict(&value));
      ASSERT_EQ(value, 4);
      ASSERT_EQ(lruk_replacer.Size(), 2);
      //测试Evict函数，当有多个帧具有+inf后向k距离时，替换器淘汰具有最早时间戳的帧，替换器size减1，frame_id为正确的帧
      LRUKReplacer lruk_replacer2(5, 5);
      for (int i = 1;i <= 5;++i) {
        lruk_replacer2.RecordAccess(i);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        lruk_replacer2.SetEvictable(i, true);
      }
      ASSERT_EQ(true, lruk_replacer2.Evict(&value));
      ASSERT_EQ(value, 1);
      ASSERT_EQ(lruk_replacer2.Size(), 4);
      ASSERT_EQ(true, lruk_replacer2.Evict(&value));
      ASSERT_EQ(value, 2);
      ASSERT_EQ(lruk_replacer2.Size(), 3);
      ASSERT_EQ(true, lruk_replacer2.Evict(&value));
      ASSERT_EQ(value, 3);
      ASSERT_EQ(lruk_replacer2.Size(), 2);
      ASSERT_EQ(true, lruk_replacer2.Evict(&value));
      ASSERT_EQ(value, 4);
      ASSERT_EQ(lruk_replacer2.Size(), 1);
      ASSERT_EQ(true, lruk_replacer2.Evict(&value));
      ASSERT_EQ(value, 5);
      ASSERT_EQ(lruk_replacer2.Size(), 0);
    }
    */
}  // namespace bustub
