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

#include "container/hash/extendible_hash_table.h"
#include "common/logger.h"
#include "storage/page/page.h"

namespace bustub {

template <typename K, typename V>
ExtendibleHashTable<K, V>::ExtendibleHashTable(size_t bucket_size)
    : global_depth_(0), bucket_size_(bucket_size), num_buckets_(1) {
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
  return GetGlobalDepthInternal();
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::GetGlobalDepthInternal() const -> int {
  return global_depth_;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::GetLocalDepth(int dir_index) const -> int {
  return GetLocalDepthInternal(dir_index);
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::GetLocalDepthInternal(int dir_index) const -> int {
  return dir_[dir_index]->GetDepth();
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::GetNumBuckets() const -> int {
  return GetNumBucketsInternal();
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::GetNumBucketsInternal() const -> int {
  return num_buckets_;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Find(const K &key, V &value) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  std::shared_ptr<Bucket> bucket;
  bool result = (FindBucket(key, bucket) && bucket->Find(key, value)) ? 1 : 0;
  return result;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Remove(const K &key) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;
  bool result = (FindBucket(key, bucket) && bucket->Remove(key)) ? 1 : 0;
  return result;
}

template <typename K, typename V>
void ExtendibleHashTable<K, V>::DoubleDirectory() {
  for (int i = 0; i < pow(2, global_depth_); i++) {
    dir_.push_back(dir_[i]);
  }  // end for
  ++global_depth_;
}  // end DoubleDirectory

template <typename K, typename V>
void ExtendibleHashTable<K, V>::InsertByDoubleDir(std::shared_ptr<Bucket> bucket, const K &key) {
  while (bucket->IsFull()) {
    DoubleDirectory();
    RedistributeBucket(bucket);
    // global_depth_ is changed, so we need to recompute index_of_key
    bucket = dir_[IndexOf(key)];
  }
}  // end InsertByDoubleDir

template <typename K, typename V>
void ExtendibleHashTable<K, V>::Insert(const K &key, const V &value) {
  V temp;
  std::shared_ptr<bustub::ExtendibleHashTable<K, V>::Bucket> bucket;

  // update value
  if (Find(key, temp)) {
    std::scoped_lock<std::mutex> lock(latch_);
    FindBucket(key, bucket);
    bucket->Insert(key, value);
  } else {  // insert value
    std::scoped_lock<std::mutex> lock(latch_);
    bucket = dir_[IndexOf(key)];
    // Double size of dir_ and redistribute bucket
    if (bucket->IsFull() && bucket->GetDepth() == global_depth_) {
      InsertByDoubleDir(bucket, key);
      dir_[IndexOf(key)]->Insert(key, value);
    } else if (bucket->IsFull() && bucket->GetDepth() < global_depth_) {
      // Redistribute bucket
      RedistributeBucket(bucket);
      while (dir_[IndexOf(key)]->IsFull()) {
        if (dir_[IndexOf(key)]->GetDepth() < global_depth_) {
          RedistributeBucket(dir_[IndexOf(key)]);
        } else {
          InsertByDoubleDir(dir_[IndexOf(key)], key);
        }
      }  // end third if
      dir_[IndexOf(key)]->Insert(key, value);
    } else {
      bucket->Insert(key, value);
    }  // end second if
  }    // end first if
}  // end Insert

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::FindBucket(const K &key, std::shared_ptr<Bucket> &bucket) -> bool {
  auto index = IndexOf(key);
  if (index > dir_.size()) {
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
  auto new_index = old_index + pow(2, bucket->GetDepth());
  bucket->IncrementDepth();
  global_depth_ = bucket->GetDepth();
  std::shared_ptr<Bucket> new_bucket = std::make_shared<Bucket>(bucket_size_, bucket->GetDepth());
  num_buckets_++;

  for (auto it = bucket->GetItems().begin(); it != bucket->GetItems().end(); it++) {
    // Move the item to the new bucket
    if (IndexOf(it->first) != old_index) {
      if (new_bucket->Insert(it->first, it->second)) {
        to_be_moved.push_back(it->first);
      }
    }
  }
  // Delate the item in the old bucket
  for (auto it : to_be_moved) {
    bucket->Remove(it);
  }
  while (new_index < dir_.size()) {
    dir_[new_index] = new_bucket;
    new_index += pow(2, bucket->GetDepth());
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
  for (auto it = list_.begin(); it != list_.end(); it++) {
    if (it->first == key) {
      list_.erase(it);
      return true;
    }
  }
  return false;
}

template <typename K, typename V>
auto ExtendibleHashTable<K, V>::Bucket::Insert(const K &key, const V &value) -> bool {
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
