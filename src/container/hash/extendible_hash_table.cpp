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
#include <math.h>
#include "common/logger.h"
#include "container/hash/extendible_hash_table.h"
#include "storage/page/page.h"

namespace bustub {

template <typename K, typename V>
ExtendibleHashTable<K, V>::ExtendibleHashTable(size_t bucket_size)
    : global_depth_(0), bucket_size_(bucket_size), num_buckets_(1) {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  std::shared_ptr<Bucket> first_bucket = std::make_shared<Bucket>(bucket_size_, global_depth_);
  dir_.push_back(first_bucket);
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::IndexOf(const K &key) -> size_t {
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
auto ExtendibleHashTable<K, V>::Find(const K &key, V &value) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  std::shared_ptr<Bucket> bucket;
  return (FindBucket(key, bucket) && bucket->Find(key, value)) ? 1 : 0;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Remove(const K &key) -> bool {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;
  return (FindBucket(key, bucket) && bucket->Remove(key)) ? 1 : 0;
}

template <typename K, typename V>
void ExtendibleHashTable<K, V>::DoubleDirectory(){
  for (int i = 0; i < pow(2,global_depth_); i++) {
    dir_.push_back(dir_[i]);
  }// end for
  ++global_depth_;
}// end DoubleDirectory

template <typename K, typename V>
void ExtendibleHashTable<K, V>::InsertByDoubleDir(std::shared_ptr<Bucket> bucket,const K &key){
  while(bucket->IsFull()){
    DoubleDirectory();
    RedistributeBucket(bucket);
    //global_depth_ is changed, so we need to recompute index_of_key
    LOG_INFO("Doublue dir,new global_depth is %d", global_depth_);
    bucket = dir_[IndexOf(key)];
  }
}// end InsertByDoubleDir

template <typename K, typename V>
void ExtendibleHashTable<K, V>::Insert(const K &key, const V &value) {
  std::unique_lock<std::mutex> lock(latch_, std::try_to_lock_t());
  V temp;
  std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;
  // update value
  if (Find(key, temp)) {
    FindBucket(key, bucket);
    bucket->Insert(key, value);
    LOG_INFO("update value in bucket %zu,key is %p", IndexOf(key),&key);
  } else {  // insert value
    bucket = dir_[IndexOf(key)];
    //Double size of dir_ and redistribute bucket
    if (bucket->IsFull() && bucket->GetDepth() == global_depth_) {
        InsertByDoubleDir(bucket,key);
        if(dir_[IndexOf(key)]->Insert(key, value)){
          LOG_INFO("Inert value doublue dir, buckte %zu ,new global_depth is %d", IndexOf(key), global_depth_);
        } else {
          LOG_DEBUG("Falid to insert value doublue dir, buckte %zu ,new global_depth is %d", IndexOf(key), global_depth_);
        }
    } else if (bucket->IsFull() && bucket->GetDepth() < global_depth_) {
      // Redistribute bucket
      RedistributeBucket(bucket);
      while(bucket->IsFull()){
        if(bucket->GetDepth() < global_depth_){
          RedistributeBucket(bucket);
        }else{
          InsertByDoubleDir(bucket,key);
        }
      }// end third if
      if(dir_[IndexOf(key)]->Insert(key, value)){
        LOG_INFO("Inert value separate bucket, buckte=%zu", IndexOf(key));
      } else {
        LOG_DEBUG("Falid to insert value separate bucket, buckte=%zu", IndexOf(key));
      }
    } else {
      if(bucket->Insert(key, value)){
        LOG_INFO("Inert value drectly, buckte=%zu", IndexOf(key));
      } else {
        LOG_DEBUG("Falid to insert value drectly, buckte=%zu", IndexOf(key));
      }
    }// end second if
  }// end first if
}// end Insert

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::FindBucket(const K &key, std::shared_ptr<Bucket> &bucket) -> bool {
  auto index = IndexOf(key);
  if (index > dir_.size()) {
    LOG_INFO("index of key is out of range");
    return false;
  }
  bucket = dir_[index];
  return true;
}

template <typename K, typename V>
void ExtendibleHashTable<K, V>::RedistributeBucket(std::shared_ptr<Bucket> bucket) {
  std::vector<K> to_be_moved;
  auto global_depth_register = GetGlobalDepthInternal();
  global_depth_ = bucket->GetDepth();
  auto old_index = IndexOf(bucket->GetItems().begin()->first);
  auto new_index = old_index+pow(2,bucket->GetDepth());
  LOG_INFO("Bucket %zu split to %zu and %d", old_index, old_index, (int)new_index);
  bucket->IncrementDepth();
  global_depth_ = bucket->GetDepth();
  std::shared_ptr<Bucket> new_bucket = std::make_shared<Bucket>(bucket_size_, bucket->GetDepth());
  num_buckets_++;

  for (auto it = bucket->GetItems().begin(); it != bucket->GetItems().end(); it++) {
    // Move the item to the new bucket
    if (IndexOf(it->first) != old_index) {
      if(new_bucket->Insert(it->first, it->second)){
        to_be_moved.push_back(it->first);
        LOG_INFO("Bucket %zu remove to bucket %d", old_index, (int)new_index);
      } else {
        LOG_DEBUG("Failed to insert key frome bucket %zu to bucket %d", old_index, (int)new_index);
      }
    }
  }
  // Delate the item in the old bucket
  for (auto it : to_be_moved) {
    if(!bucket->Remove(it)){
      LOG_INFO("Failed to remove element failed!");
    }
  }
  while(new_index<dir_.size()){
    dir_[new_index] = new_bucket;
    new_index += pow(2,bucket->GetDepth());
  }
  global_depth_ = global_depth_register;
}

//===--------------------------------------------------------------------===//
// Bucket
//===--------------------------------------------------------------------===//
template <typename K, typename V>
ExtendibleHashTable<K, V>::Bucket::Bucket(size_t array_size, int depth) : size_(array_size), depth_(depth) {}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Bucket::Find(const K &key, V &value) -> bool {
  // 在list<std::pair<K, V>>
  // list_里找包含该key的pair.first,如果找到了，返回true，并把pair.second赋给value,找不到返回false
  for (auto &it : list_) {
    if (it.first == key) {
      value = it.second;
      return true;
    }
  }
  return false;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Bucket::Remove(const K &key) -> bool {
  // 遍历list_如果在list_找到包含key的pair.first,删除该pair,返回true.如果没有找到,返回false
  for (auto it = list_.begin(); it != list_.end(); it++) {
    if (it->first == key) {
      list_.erase(it);
      LOG_INFO("Successful remove element");
      return true;
    }
  }
  return false;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Bucket::Insert(const K &key, const V &value) -> bool {
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
  }
  return false;
}

template class ExtendibleHashTable<page_id_t, Page *>;
template class ExtendibleHashTable<Page *, std::list<Page *>::iterator>;
template class ExtendibleHashTable<int, int>;
// test purpose
template class ExtendibleHashTable<int, std::string>;
template class ExtendibleHashTable<int, std::list<int>::iterator>;

}  // namespace bustub
