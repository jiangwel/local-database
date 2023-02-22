//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_hash_table.cpp
//
// Identification: src/container/hash/extendible_hash_table.cpp
//
// Copyright (c) 2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cstdlib>
#include <functional>
#include <list>
#include <utility>

#include "common/logger.h"
#include "container/hash/extendible_hash_table.h"
#include "storage/page/page.h"

namespace bustub {

  template <typename K, typename V>
  ExtendibleHashTable<K, V>::ExtendibleHashTable(size_t bucket_size)
    : global_depth_(0), bucket_size_(bucket_size), num_buckets_(1) {
      std::unique_lock<std::mutex> lock(latch_,std::try_to_lock_t());
      std::shared_ptr<Bucket> first_bucket= std::make_shared<Bucket>(bucket_size_,global_depth_);
      dir_.push_back(first_bucket);
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::IndexOf(const K& key) -> size_t {
    int mask = (1 << global_depth_) - 1;
    return std::hash<K>()(key) & mask;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetGlobalDepth() const -> int {
    std::scoped_lock<std::mutex> lock(latch_);
    return GetGlobalDepthInternal();
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetGlobalDepthInternal() const -> int {
    return global_depth_;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetLocalDepth(int dir_index) const -> int {
    std::scoped_lock<std::mutex> lock(latch_);
    return GetLocalDepthInternal(dir_index);
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetLocalDepthInternal(int dir_index) const -> int {
    return dir_[dir_index]->GetDepth();
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetNumBuckets() const -> int {
    std::scoped_lock<std::mutex> lock(latch_);
    return GetNumBucketsInternal();
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::GetNumBucketsInternal() const -> int {
    return num_buckets_;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::Find(const K& key, V& value) -> bool {
    std::unique_lock<std::mutex> lock(latch_,std::try_to_lock_t());
    std::shared_ptr<Bucket> bucket;
    return (FindBucket(key,bucket) && bucket->Find(key,value)) ? true : false;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::Remove(const K& key) -> bool {
    std::unique_lock<std::mutex> lock(latch_,std::try_to_lock_t());
    std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;
    return (FindBucket(key,bucket) && bucket->Remove(key) ? true : false);
  }

  template <typename K, typename V>
  void ExtendibleHashTable<K, V>::Insert(const K& key, const V& value) {
    std::unique_lock<std::mutex> lock(latch_,std::try_to_lock_t());
    V temp;
    std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;
    // update value
    if(Find(key,temp)){
      FindBucket(key,bucket);
      // 在bucket里找到key，然后更新value
      bucket->Insert(key,value);
    } else { // insert value
      auto index_of_key = IndexOf(key);
      bucket = dir_[index_of_key];
      if(bucket->IsFull() && bucket->GetDepth() == global_depth_){
        ++global_depth_;
        for(int i = 0;i < num_buckets_;i++){dir_.push_back(dir_[i]);}
        num_buckets_*=2;
        RedistributeBucket(bucket,index_of_key);
      } else if(bucket->IsFull() && bucket->GetDepth() + 1 == global_depth_){
        RedistributeBucket(bucket,index_of_key);
      }
      bucket->Insert(key,value);
    }
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::FindBucket(const K &key,std::shared_ptr<Bucket> &bucket) -> bool{
    int index = IndexOf(key);
    if(index >= num_buckets_){
      return false;
    } else {
      bucket = dir_[index];
      return true;
    }
  }

  template <typename K, typename V>
  void ExtendibleHashTable<K, V>::RedistributeBucket(std::shared_ptr<Bucket> bucket,size_t old_index){
    bucket->IncrementDepth();
    std::vector<typename std::list<std::pair<K, V>>::iterator> to_be_moved;
    //仅仅遍历bucket
    for(auto it = bucket->GetItems().begin();it != bucket->GetItems().end();it++){
      auto new_index = IndexOf(it->first);
      // 如果新的index和旧的index不一样，在dir_[new_index]里生成一个新的bucket，然后把当前的pair插入到新的bucket里
      if(new_index != old_index&&dir_[new_index] == dir_[old_index]){
        std::shared_ptr<Bucket> new_bucket = std::make_shared<Bucket>(bucket_size_,GetGlobalDepthInternal());
        dir_[new_index] = new_bucket;
      }
      if(new_index != old_index){
        dir_[new_index]->GetItems().push_back(*it);
        to_be_moved.push_back(it);
      }
    }
    for(auto it:to_be_moved){bucket->GetItems().erase(it);}
  }

  //===--------------------------------------------------------------------===//
  // Bucket
  //===--------------------------------------------------------------------===//
  template <typename K, typename V>
  ExtendibleHashTable<K, V>::Bucket::Bucket(size_t array_size, int depth): size_(array_size), depth_(depth) {}

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::Bucket::Find(const K& key, V& value) -> bool {
    // 在list<std::pair<K, V>> list_里找包含该key的pair.first,如果找到了，返回true，并把pair.second赋给value,找不到返回false
    for (auto &it : list_) {
      if (it.first == key) {
        value = it.second;
        return true;
      }
    }
    return false;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::Bucket::Remove(const K& key) -> bool {
    // 遍历list_如果在list_找到包含key的pair.first,删除该pair,返回true.如果没有找到,返回false
    for (auto it = list_.begin(); it != list_.end(); it++) {
      if (it->first == key) {
        list_.erase(it);
        return true;
      }
    }
    return false;
  }

  template <typename K, typename V>
  auto ExtendibleHashTable<K, V>::Bucket::Insert(const K& key, const V& value) -> bool {
    // 遍历list_如果在list_找到包含key的pair.first,把pair.second赋值为value,返回true.如果没有找到且IsFull()返回了false,插入pair(key,value),返回true.如果没有找到且IsFull()返回了true,返回false
    for (auto &it : list_) {
      if (it.first == key) {
        it.second = value;
        return true;
      }
    }
    if (!IsFull()) {
      list_.push_back(std::make_pair(key, value));
      return true;
    } else {
      return false;
    }
  }

  template class ExtendibleHashTable<page_id_t, Page*>;
  template class ExtendibleHashTable<Page*, std::list<Page*>::iterator>;
  template class ExtendibleHashTable<int, int>;
  // test purpose
  template class ExtendibleHashTable<int, std::string>;
  template class ExtendibleHashTable<int, std::list<int>::iterator>;

}  // namespace bustub
