#include <string>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "storage/index/b_plus_tree.h"
#include "storage/page/header_page.h"

#define PrintEntryFunInfo
// #define PrintGetLeafInfo
// #define PrintLogInfo
// #define PrintThreadInfo

// #define TestCode

namespace bustub {
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                          int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      buffer_pool_manager_(buffer_pool_manager),
      comparator_(comparator),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size),
      root_page_id_(INVALID_PAGE_ID) {
        #ifdef PrintEntryFunInfo
        LOG_INFO("BPlusTree: leaf_max_size_=%d, internal_max_size_=%d", leaf_max_size_, internal_max_size_);
        #endif
      }

/*
 * Helper function to decide whether current b+tree is empty
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::IsEmpty() const -> bool { return root_page_id_ == INVALID_PAGE_ID; }
/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetValue(const KeyType &key, std::vector<ValueType> *result, Transaction *transaction) -> bool {
  root_page_id_latch_.RLock();
  #ifdef PrintLogInfo
  LOG_INFO("GetValue: key=%ld", key.ToString());
  #endif
  Page *root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  if(transaction != nullptr){
    root_page->RLatch();
    transaction->GetPageSet()->push_back(root_page);
  }
  root_page_id_latch_.RUnlock();

  int temp = -1;
  int *index = &temp;
  LeafPage *leaf = GetLeaf(key, index, OperateType::Find, transaction, root_page);
  // key is found
  if (*index != -1) {
    result->push_back(leaf->ValueAt(*index));
    if (transaction != nullptr) {
      transaction->GetPageSet()->back()->RUnlatch();
      #ifdef PrintThreadInfo
      LOG_INFO("RUnlatch p%d thread %zu", transaction->GetPageSet()->back()->GetPageId(),
               std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      transaction->GetPageSet()->clear();
    }
    if (!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false)) {
      LOG_DEBUG("Insert: unpin leaf1 page failed");
    }
    return true;
  }
  if (transaction != nullptr) {
    transaction->GetPageSet()->back()->RUnlatch();
    #ifdef PrintThreadInfo
    LOG_INFO("RUnlatch p%d thread %zu", transaction->GetPageSet()->back()->GetPageId(),
             std::hash<std::thread::id>{}(transaction->GetThreadId()));
    #endif
    transaction->GetPageSet()->pop_back();
  }
  return false;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * if current tree is empty, start new tree, update root page id and insert
 * entry, otherwise insert into leaf page.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Insert(const KeyType &key, const ValueType &value, Transaction *transaction) -> bool {
  root_page_id_latch_.WLock();
  #ifdef PrintEntryFunInfo
  LOG_INFO("@Insert key is: %ld rid.GetPageId:%d thread %zu", key.ToString(), value.GetPageId(),
           std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  #ifdef TestCode
  auto test_key = key.ToString();
  if(test_key==1000){

    LOG_INFO("hello");
    
  }
  #endif
  // empty tree
  if (root_page_id_ == INVALID_PAGE_ID) {
    page_id_t new_root_page_id;
    auto root = reinterpret_cast<LeafPage *>(buffer_pool_manager_->NewPage(&new_root_page_id)->GetData());
    root_page_id_ = new_root_page_id;
    root->Init(root_page_id_, INVALID_PAGE_ID, leaf_max_size_);
    InsertNode(reinterpret_cast<BPlusTreePage *>(root), key, value);
    UpdateRootPageId(0);

    if (!buffer_pool_manager_->UnpinPage(root->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin root page failed");
    }
    root_page_id_latch_.WUnlock();
    return true;
  }
  std::shared_ptr<std::deque<bustub::Page *>> page_set = nullptr;
  if (transaction != nullptr) {
    page_set = transaction->GetPageSet();
  }
  #ifdef PrintThreadInfo
  LOG_INFO("T get root page thread %zu", std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif 
  auto root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  root_page->WLatch();
  #ifdef PrintThreadInfo
  LOG_INFO("S get root page thread %zu", std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  page_set->push_back(root_page);
  // Get leaf1
  int temp = -1;
  int *index = &temp;
  LeafPage *leaf1 = GetLeaf(key, index, OperateType::Insert, transaction, root_page);
  bool leaf1IsFull = leaf1->GetSize() +1 == leaf_max_size_;
  if(!leaf1IsFull){
    root_page_id_latch_.WUnlock();
  }
  // Duplicate key, return false
  if (*index != -1) {
    if(leaf1IsFull){
      root_page_id_latch_.WUnlock();
    }
    if (transaction != nullptr) {
      for (auto p : *page_set) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        p->WUnlatch();
        if (p->GetPageId() == root_page_id_) {
          #ifdef PrintRootInfo
          LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
          #ifdef PrintRootInfo
          LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
        }
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
      page_set->clear();
      
      // protect root_page_id
    }
    if (!buffer_pool_manager_->UnpinPage(leaf1->GetPageId(), false)) {
      LOG_DEBUG("Insert: unpin leaf1 page failed");
    }
    #ifdef PrintLogInfo
    LOG_INFO("Insert: duplicate key");
    #endif
    return false;
  }

  auto node1 = reinterpret_cast<BPlusTreePage *>(leaf1);

  // node1 is not full, insert directly
  if (!leaf1IsFull) {
    InsertNode(node1, key, value);
    if (transaction != nullptr) {
      #ifdef PrintThreadInfo
      LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
               std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      page_set->back()->WUnlatch();
      #ifdef PrintThreadInfo
      LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
               std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      page_set->clear();
      // protect root_page_id
      if (node1->GetPageId() == root_page_id_) {
        #ifdef PrintRootInfo
        LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
        #ifdef PrintRootInfo
        LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
    }
    if (!buffer_pool_manager_->UnpinPage(node1->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin leaf1 page failed");
    }
    return true;
  }

  // node1 is full Create a new leaf
  page_id_t leaf2_page_id;
  #ifdef PrintThreadInfo
  LOG_INFO("T get leaf2 page thread %zu", std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  auto leaf2_page = buffer_pool_manager_->NewPage(&leaf2_page_id);
  #ifdef PrintThreadInfo
  LOG_INFO("S get leaf2 page thread %zu", std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  if (transaction != nullptr) {
    #ifdef PrintThreadInfo
    LOG_INFO("T WLatch p%d thread %zu", leaf2_page->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
    #endif
    leaf2_page->WLatch();
    #ifdef PrintThreadInfo
    LOG_INFO("S WLatch p%d thread %zu", leaf2_page->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
    #endif
    page_set->push_back(leaf2_page);
  }
  auto leaf2 = reinterpret_cast<LeafPage *>(leaf2_page->GetData());
  leaf2->Init(leaf2_page_id, leaf1->GetParentPageId(), leaf_max_size_);
  auto node2 = reinterpret_cast<BPlusTreePage *>(leaf2);

  // splite
  InsertNode(node1, key, value);
  leaf2->SetNextPageId(leaf1->GetNextPageId());
  leaf1->SetNextPageId(leaf2->GetPageId());
  // move to leaf2
  for (int i = leaf1->GetSize() / 2; i < leaf1->GetSize(); ++i) {
    leaf2->SetPairAt(leaf2->GetSize(), {leaf1->KeyAt(i), leaf1->ValueAt(i)});
    leaf2->IncreaseSize(1);
  }
  int increase_size = leaf1->GetSize() - leaf1->GetSize() / 2;
  leaf1->IncreaseSize(-increase_size);

  // update parent
  KeyType first_key = leaf2->KeyAt(0);
  RID rid(leaf2->GetPageId(), leaf2->GetPageId() & 0xFFFFFFFF);
  BPlusTree::InsertParent(node1, node2, first_key, rid, transaction);

  return true;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertNode(BPlusTreePage *node, const KeyType &key, const ValueType &value) {
  #ifdef PrintLogInfo
  LOG_INFO("InsertNode: key=%ld value=%d page id=%d thread %zu", key.ToString(), value.GetPageId(), node->GetPageId(), std::hash<std::thread::id>{}(std::this_thread::get_id()));
  #endif
  int value_int = value.GetSlotNum();
  // #insert leaf
  if (node->IsLeafPage()) {
    auto leaf = reinterpret_cast<LeafPage *>(node);

    // leaf is empty or key < first key in leaf
    if (leaf->GetSize() == 0 || comparator_(key, leaf->KeyAt(0)) == -1) {
      if (!leaf->SetPairAt(0, MappingType(key, value))) {
        LOG_DEBUG("InsertNode: set pair failed 2");
      }
      leaf->IncreaseSize(1);
      return;
    }

    for (int i = 1; i < leaf->GetSize(); i++) {
      // it->first > key
      if (comparator_(key, leaf->KeyAt(i)) == -1) {
        if (!leaf->SetPairAt(i, MappingType(key, value))) {
          LOG_DEBUG("InsertNode: set pair failed 3");
        }
        leaf->IncreaseSize(1);
        return;
      }
    }
    // key > last key in leaf
    if (!leaf->SetPairAt(leaf->GetSize(), MappingType(key, value))) {
      LOG_DEBUG("InsertNode: set pair failed 6");
    }
    leaf->IncreaseSize(1);
    return;
  }
  // #end insert leaf

  // #insert internal
  auto internal = reinterpret_cast<InternalPage *>(node);
  // internal is no key or key < first key in internal
  if (internal->GetSize() == 1 || comparator_(key, internal->KeyAt(1)) == -1) {
    if (!internal->SetPairAt(1, std::make_pair(key, value_int))) {
      LOG_DEBUG("InsertNode: set pair failed 4");
    }
    internal->IncreaseSize(1);
    return;
  }
  // insert
  // fist key is invalid and index 1 compared,so start at 2
  for (int i = 2; i < internal->GetSize(); i++) {
    // key < it->first
    if (comparator_(key, internal->KeyAt(i)) == -1) {
      if (!internal->SetPairAt(i, std::make_pair(key, value_int))) {
        LOG_DEBUG("InsertNode: set pair failed 5");
      }
      internal->IncreaseSize(1);
      return;
    }
  }
  // key > last key in internal
  // fist key is invalid so size+1
  if (!internal->SetPairAt(internal->GetSize(), std::make_pair(key, value_int))) {
    LOG_DEBUG("InsertNode: set pair failed 7");
  }
  internal->IncreaseSize(1);
  // #end insert internal
}

INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetLeaf(const KeyType &key, int *index, OperateType operator_type, Transaction *transaction,Page* node_page)
    -> LeafPage * {
  #ifdef PrintLogInfo
  LOG_INFO("GetLeaf");
  #endif
  auto node = reinterpret_cast<BPlusTreePage *>(node_page->GetData());
  // LockAndUnlock(node_page, node, OperateType::Other, transaction);
  while (!node->IsLeafPage()) {
    bool found = false;
    auto node_as_internal = reinterpret_cast<InternalPage *>(node);
    page_id_t next_page_id;

    // find key <= input_kay
    for (int i = 1; i < node_as_internal->GetSize(); i++) {
      if (comparator_(key, node_as_internal->KeyAt(i)) != 1) {
        // is key == input_kay?
        next_page_id = comparator_(key, node_as_internal->KeyAt(i)) == 0 ? node_as_internal->ValueAt(i)
                                                                         : node_as_internal->ValueAt(i - 1);
        found = true;
        break;
      }
    }
    // all key > input_kay
    next_page_id = !found ? (node_as_internal->ValueAt(node_as_internal->GetSize() - 1)) : next_page_id;
    node_page = buffer_pool_manager_->BufferPoolManager::FetchPage(next_page_id);
    node = reinterpret_cast<BPlusTreePage *>(node_page->GetData());
    LockAndUnlock(node_page, node, operator_type, transaction);
    if (!buffer_pool_manager_->UnpinPage(node_as_internal->GetPageId(), false)) {
      LOG_DEBUG("GetLeaf: unpin internal page failed");
    }
  }

  auto leaf = reinterpret_cast<LeafPage *>(node);
  // check if key exists in leaf
  for (int i = 0; i < leaf->GetSize(); i++) {
    if (comparator_(key, leaf->KeyAt(i)) == 0) {
      *index = i;
      return leaf;
    }
  }
  *index = -1;
  return leaf;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::LockAndUnlock(Page *page, BPlusTreePage *node, OperateType operator_type,
                                   Transaction *transaction) {
  #ifdef PrintLogInfo
  LOG_INFO("LockAndUnlock");
  #endif
  if (transaction == nullptr) {
    return;
  }

  auto page_set = transaction->GetPageSet();
  if (operator_type == OperateType::Find) {
    page->RLatch();
    #ifdef PrintThreadInfo
    LOG_INFO("Rlatch p%d thread %zu", page->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
    #endif
    page_set->back()->RUnlatch();
    #ifdef PrintThreadInfo
    LOG_INFO("RUnlatch p%d thread %zu", page_set->back()->GetPageId(),
             std::hash<std::thread::id>{}(transaction->GetThreadId()));
    #endif
    page_set->pop_back();
    page_set->push_back(page);
    return;
  }
  #ifdef PrintThreadInfo
  LOG_INFO("T WLatch p%d thread %zu", page->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  page->WLatch();
  #ifdef PrintThreadInfo
  LOG_INFO("S WLatch p%d thread %zu", page->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  // leaf put at most max_size-1 elements
  int add_num = node->IsLeafPage() ? -1 : 0;
  // safe
  if ((operator_type == OperateType::Insert && node->GetSize() < node->GetMaxSize() + add_num) ||
      (operator_type == OperateType::Delete && node->GetSize() > node->GetMinSize())) {
    
    for (auto p : *page_set) {
      #ifdef PrintThreadInfo
      LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      p->WUnlatch();
      #ifdef PrintThreadInfo
      LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      if (p->GetPageId() == root_page_id_) {
        #ifdef PrintRootInfo
        LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
        #ifdef PrintRootInfo
        LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
    }
    page_set->clear();
  }
  page_set->push_back(page);

}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertParent(BPlusTreePage *page1, BPlusTreePage *page2, const KeyType &key,
                                  const ValueType &value, Transaction *transaction) {
  #ifdef PrintLogInfo
  LOG_INFO("InsertParent: key=%ld value=%d page1 id=%d page2 id=%d thread %zu", key.ToString(), value.GetPageId(),
           page1->GetPageId(), page2->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  std::shared_ptr<std::deque<bustub::Page *>> page_set = nullptr;
  if (transaction != nullptr) {
    page_set = transaction->GetPageSet();
  }

  if (page1->IsRootPage()) {
    // Create a new root
    page_id_t root_page_id;
    auto new_root = reinterpret_cast<InternalPage *>(buffer_pool_manager_->NewPage(&root_page_id)->GetData());
    new_root->Init(root_page_id, INVALID_PAGE_ID, internal_max_size_);

    // Insert page1,key,page2 into new root
    KeyType invalid_key;
    invalid_key.SetFromInteger(-1);
    if (!new_root->SetPairAt(0, std::make_pair(invalid_key, page1->GetPageId()))) {
      LOG_DEBUG("InsertParent: set pair failed 1");
    }
    new_root->IncreaseSize(1);
    if (!new_root->SetPairAt(1, std::make_pair(key, page2->GetPageId()))) {
      LOG_DEBUG("InsertParent: set pair failed 2");
    }
    new_root->IncreaseSize(1);

    page1->SetParentPageId(root_page_id);
    page2->SetParentPageId(root_page_id);
    auto old_root_page_id = root_page_id_;
    root_page_id_ = root_page_id;
    UpdateRootPageId(1);
    root_page_id_latch_.WUnlock();

    // unlatch all
    if (transaction != nullptr) {
      for (auto p : *page_set) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        p->WUnlatch();
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        if(p->GetPageId() == old_root_page_id){
          #ifdef PrintRootInfo
          LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
          #ifdef PrintRootInfo
          LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
        }
        buffer_pool_manager_->UnpinPage(p->GetPageId(), true);
      }
      page_set->clear();
    }
    return;
  }

  auto parent_page = buffer_pool_manager_->BufferPoolManager::FetchPage(page1->GetParentPageId());
  auto *parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
  // parent is full
  if (parent->GetSize() == parent->GetMaxSize()) {
    #ifdef PrintLogInfo
    LOG_INFO("parent is full");
    #endif
    // create a new page
    page_id_t parent_page_prime_id;
    auto parent_prime_page = buffer_pool_manager_->NewPage(&parent_page_prime_id);
    auto parent_prime = reinterpret_cast<InternalPage *>(parent_prime_page->GetData());
    parent_prime->Init(parent_page_prime_id, parent->GetParentPageId(), internal_max_size_);

    // split
    InsertNode(reinterpret_cast<BPlusTreePage *>(parent), key, value);
    auto half_index = parent->GetSize() / 2;
    auto k_prime = parent->KeyAt(half_index);
    for (int i = half_index; i < parent->GetSize(); ++i) {
      // change parent page id
      auto move_page =
          reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(parent->ValueAt(i))->GetData());
      move_page->SetParentPageId(parent_prime->GetPageId());
      if (!buffer_pool_manager_->UnpinPage(move_page->GetPageId(), true)) {
        LOG_DEBUG("Unpin page failed");
      }
      // move to parent_prime
      parent_prime->SetPairAt(parent_prime->GetSize(), {parent->KeyAt(i), parent->ValueAt(i)});
      parent_prime->IncreaseSize(1);
    }
    int increase_size = parent->GetSize() - half_index;
    parent->IncreaseSize(-increase_size);
    // end split

    RID rid(parent_page_prime_id, parent_page_prime_id & 0xFFFFFFFF);

    InsertParent(parent, parent_prime, k_prime, rid, transaction);
    return;
  }
  root_page_id_latch_.WUnlock();
  // parent is not full
  InsertNode(parent, key, value);
  // unlatch all
  if (transaction != nullptr) {
    for (auto p : *page_set) {
      #ifdef PrintThreadInfo
      LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      p->WUnlatch();
      #ifdef PrintThreadInfo
      LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
      #endif
      if (p->GetPageId() == root_page_id_) {
        #ifdef PrintRootInfo
        LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                  std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
        #ifdef PrintRootInfo
        LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                  std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
      buffer_pool_manager_->UnpinPage(p->GetPageId(), true);
    }
    page_set->clear();
  }
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immdiately.
 * If not, User needs to first find the right leaf page as deletion target, then
 * delete entry from leaf page. Remember to deal with redistribute or merge if
 * necessary.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key, Transaction *transaction) {
  root_page_id_latch_.WLock();
  #ifdef PrintEntryFunInfo
  LOG_INFO("@Remove: key=%ld thread: %zu", key.ToString(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  if (root_page_id_ == INVALID_PAGE_ID) {
    root_page_id_latch_.WUnlock();
    return;
  }
  if (root_page_id_ != INVALID_PAGE_ID) {
    Page *root_page = buffer_pool_manager_->FetchPage(root_page_id_);
    root_page->WLatch();
    transaction->GetPageSet()->push_back(root_page);

    int temp = -1;
    int *index = &temp;
    LeafPage *leaf = BPlusTree::GetLeaf(key, index, OperateType::Delete, transaction, root_page);
    bool leafIsRoot = leaf->IsRootPage(),leafSizeIs1 = leaf->GetSize() == 1,leafIsInternal = !leaf->IsLeafPage();
    // if(!(leafIsRoot && leafSizeIs1 && leafIsInternal)){
    //   root_page_id_latch_.WUnlock();
    // }
    if (*index == -1) {
      if(leafIsRoot && leafSizeIs1 && leafIsInternal){
        root_page_id_latch_.WUnlock();
      }
      #ifdef PrintLogInfo
      LOG_INFO("key not found");
      #endif
      auto page_set = transaction->GetPageSet();
      if (transaction != nullptr) {
        for (auto p : *page_set) {
          #ifdef PrintThreadInfo
          LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          p->WUnlatch();
          if (p->GetPageId() == root_page_id_) {
            #ifdef PrintRootInfo
            LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                    std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
            root_page_id_latch_.WUnlock();
            #ifdef PrintRootInfo
            LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                    std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
          }
          #ifdef PrintThreadInfo
          LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(), std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
        }
        page_set->clear();
        // protect root_page_id
      }
      if (!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false)) {
        LOG_DEBUG("Insert: unpin leaf page failed");
      }
    } else {
      BPlusTree::RemoveEntry(reinterpret_cast<BPlusTreePage *>(leaf), key, transaction);
    }
  }
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::RemoveEntry(BPlusTreePage *node1, const KeyType &key, Transaction *transaction) {
  #ifdef PrintLogInfo
  LOG_INFO("RemoveEntry: key=%ld thread: %zu node1: %d", key.ToString(),
           std::hash<std::thread::id>{}(transaction->GetThreadId()), node1->GetPageId());
  #endif
  std::shared_ptr<std::deque<bustub::Page *>> page_set = nullptr;
  if (transaction != nullptr) {
    page_set = transaction->GetPageSet();
  }
  // #leaf
  if (node1->IsLeafPage()) {
    #ifdef PrintLogInfo
    LOG_INFO("node1 is leaf");
    #endif
    auto leaf = reinterpret_cast<LeafPage *>(node1);
    KeyType key_plus = leaf->KeyAt(0);
    if (!leaf->DeletePair(key, comparator_)) {
      LOG_DEBUG("RemoveEntry: delete pair failed");
      return;
    }
    bool is_key_num_not_enough = leaf->GetSize() < leaf->GetMinSize();
    if (leaf->IsRootPage() || !is_key_num_not_enough) {
      #ifdef PrintLogInfo
      LOG_INFO("leaf is root or size >= minsize");
      #endif
      // unlatch node1
      if (transaction != nullptr) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->back()->WUnlatch();
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->pop_back();
        #ifdef PrintRootInfo
        LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        root_page_id_latch_.WUnlock();
        #ifdef PrintRootInfo
        LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
      return;
    } else if (is_key_num_not_enough) {
      LeafPage *leaf_plus = leaf;
      auto parent_page = buffer_pool_manager_->FetchPage(leaf->GetParentPageId());
      Page *sibling_page = parent_page;
      auto *parent = reinterpret_cast<InternalPage *>(parent_page->GetData());

      // find leaf_plus
      #ifdef PrintLogInfo
      LOG_INFO("find leaf_plus");
      #endif
      if (leaf->GetPageId() != parent->ValueAt(parent->GetSize() - 1)) {
        sibling_page = buffer_pool_manager_->FetchPage(leaf->GetNextPageId());
        leaf_plus = reinterpret_cast<LeafPage *>(sibling_page->GetData());
        // key_plus maybe not equal to leaf_plus.kay[0]
        for (int i = 1; i < parent->GetSize(); i++) {
          if (parent->ValueAt(i) == leaf_plus->GetPageId()) {
            key_plus = parent->KeyAt(i);
            break;
          }
        }
      } else {
        for (int i = 1; i < parent->GetSize(); i++) {
          if (parent->ValueAt(i) == leaf->GetPageId()) {
            auto leaf_plus_page_id = parent->ValueAt(i - 1);
            sibling_page = buffer_pool_manager_->FetchPage(leaf_plus_page_id);
            leaf_plus = reinterpret_cast<LeafPage *>(sibling_page->GetData());
            key_plus = parent->KeyAt(i);
            break;
          }
        }
      }
      if (transaction != nullptr) {
        #ifdef PrintLogInfo
        LOG_INFO("sibling_page id %d", sibling_page->GetPageId());
        #endif
        sibling_page->WLatch();
      }

      // #coalesce
      if (leaf->GetSize() + leaf_plus->GetSize() <= leaf->GetMaxSize()) {
        #ifdef PrintLogInfo
        LOG_INFO("leaf coalesce leaf is %d leaf_plus is %d", leaf->GetPageId(), leaf_plus->GetPageId());
        #endif
        if (leaf_plus->GetNextPageId() != leaf->GetPageId()) {
          std::swap(leaf, leaf_plus);
        }
        for (int i = 0; i < leaf->GetSize(); i++) {
          if (!leaf_plus->SetPairAt(leaf_plus->GetSize(), {leaf->KeyAt(i), leaf->ValueAt(i)})) {
            LOG_DEBUG("RemoveEntry: set pair failed");
          }
          leaf_plus->IncreaseSize(1);
        }
        leaf_plus->SetNextPageId(leaf->GetNextPageId());
        if (transaction != nullptr) {
          sibling_page->WUnlatch();
          // unlatch node1
          #ifdef PrintThreadInfo
          LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->back()->WUnlatch();
          #ifdef PrintThreadInfo
          LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->pop_back();

          transaction->AddIntoDeletedPageSet(leaf->GetPageId());
        }
        buffer_pool_manager_->DeletePage(leaf->GetPageId());
        if (!buffer_pool_manager_->UnpinPage(leaf_plus->GetPageId(), true)) {
          LOG_DEBUG("RemoveEntry: Unpin page failed");
        }
        BPlusTree::RemoveEntry(reinterpret_cast<BPlusTreePage *>(parent), key_plus, transaction);
        return;
      }
      // # redistribute
      else {
        #ifdef PrintLogInfo
        LOG_INFO("leaf redistribute leaf is %d leaf_plus is %d", leaf->GetPageId(), leaf_plus->GetPageId());
        #endif
        KeyType temp_key;
        ValueType temp_value;
        int insert_idx = 0;
        // leaf_plus if befor leaf
        if (leaf_plus->GetNextPageId() == leaf->GetPageId()) {
          temp_key = leaf_plus->KeyAt(leaf_plus->GetSize() - 1);
          temp_value = leaf_plus->ValueAt(leaf_plus->GetSize() - 1);
          leaf_plus->IncreaseSize(-1);

        } else {
          // leaf is before leaf_plus
          temp_key = leaf_plus->KeyAt(0);
          temp_value = leaf_plus->ValueAt(0);
          if (!leaf_plus->DeletePair(temp_key, comparator_)) {
            LOG_DEBUG("redistribute: delete pair failed");
          }
          insert_idx = leaf->GetSize();
        }
        if (!leaf->SetPairAt(insert_idx, {temp_key, temp_value})) {
          LOG_DEBUG("redistribute: set pair failed");
        }
        leaf->IncreaseSize(1);

        for (int i = 1; i < parent->GetSize(); i++) {
          if (comparator_(parent->KeyAt(i), key_plus) == 0) {
            parent->SetKeyAt(i, temp_key);
            break;
          }
        }
        // # end redistribute

        if (transaction != nullptr) {
          sibling_page->WUnlatch();
          // unlatch node1 and parent
          for (auto p : *page_set) {
            #ifdef PrintThreadInfo
            LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(),
                     std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
            p->WUnlatch();
            #ifdef PrintThreadInfo
            LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(),
                     std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
            if (p->GetPageId() == root_page_id_) {
              #ifdef PrintRootInfo
              LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                       std::hash<std::thread::id>{}(transaction->GetThreadId()));
              #endif
              // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
              #ifdef PrintRootInfo
              LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                       std::hash<std::thread::id>{}(transaction->GetThreadId()));
              #endif
            }
          }
          page_set->clear();
        }
        if (!buffer_pool_manager_->UnpinPage(parent->GetPageId(), true)) {
          LOG_DEBUG("redistribute: Unpin page failed");
        }
        if (!buffer_pool_manager_->UnpinPage(leaf_plus->GetPageId(), true)) {
          LOG_DEBUG("redistribute: Unpin page failed");
        }
        if (!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), true)) {
          LOG_DEBUG("redistribute: Unpin page failed");
        }
        return;
      }
    }
    else {
      if (transaction != nullptr) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->back()->WUnlatch();
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->pop_back();
      }
      if (!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), true)) {
        LOG_DEBUG("leaf: unpin page failed");
      }
    }
  }
  // #internal
  else {
    auto internal = reinterpret_cast<InternalPage *>(node1);
    if (!internal->DeletePair(key, comparator_)) {
      LOG_DEBUG("internal: delete pair failed");
      return;
    }

    if (internal->IsRootPage()) {
      // internal only has one child
      if (internal->GetSize() == 1) {
        #ifdef PrintLogInfo
        LOG_INFO("internal only has one child");
        #endif
        auto child_page_id = internal->ValueAt(0);
        auto child = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(child_page_id)->GetData());
        child->SetParentPageId(INVALID_PAGE_ID);
        root_page_id_ = child_page_id;
        UpdateRootPageId(1);
        root_page_id_latch_.WUnlock();
        #ifdef PrintLogInfo
        LOG_INFO("UpdateRootPageId");
        #endif

        if (!buffer_pool_manager_->UnpinPage(child_page_id, true)) {
          LOG_DEBUG("internal: unpin page failed");
        }
        #ifdef PrintLogInfo
        LOG_INFO("UnpinPage");
        #endif
        // unlatch node1(internal)
        if (transaction != nullptr) {
          transaction->AddIntoDeletedPageSet(internal->GetPageId());
          #ifdef PrintThreadInfo
          LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->back()->WUnlatch();
          #ifdef PrintThreadInfo
          LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->pop_back();
          #ifdef PrintRootInfo
          LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          // root_page_id_latch_.UnlockRootID(transaction->GetThreadId());
          #ifdef PrintRootInfo
          LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
        }

        if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), true)) {
          LOG_DEBUG("internal: unpin page failed");
        }
        #ifdef PrintLogInfo
        LOG_INFO("UnpinPage1");
        #endif
        buffer_pool_manager_->DeletePage(internal->GetPageId());
        #ifdef PrintLogInfo
        LOG_INFO("DeletePage");
        #endif
        return;
      }
      // unlatch node1
      if (transaction != nullptr) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->back()->WUnlatch();
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->pop_back();
        #ifdef PrintRootInfo
        LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        root_page_id_latch_.WUnlock();
        #ifdef PrintRootInfo
        LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
      }
      return;
    } else if (internal->GetSize() < internal->GetMinSize()) {
      // parent's key what betoween internal and internal_plus
      KeyType key_plus;
      InternalPage *internal_plus = internal;
      bool is_i_plus_before_i = true;
      auto *parent =
          reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(internal->GetParentPageId())->GetData());

      // find internal_plus
      for (int i = 0; i < parent->GetSize(); i++) {
        if (parent->ValueAt(i) == internal->GetPageId()) {
          if (i == 0) {
            key_plus = parent->KeyAt(i + 1);
            auto internal_plus_page_id = parent->ValueAt(i + 1);
            internal_plus =
                reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(internal_plus_page_id)->GetData());
            is_i_plus_before_i = false;
          } else {
            key_plus = parent->KeyAt(i);
            auto internal_plus_page_id = parent->ValueAt(i - 1);
            internal_plus =
                reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(internal_plus_page_id)->GetData());
          }
          break;
        }
      }

      // #coalesce
      if (internal->GetSize() + internal_plus->GetSize() <= internal->GetMaxSize()) {
        #ifdef PrintLogInfo
        LOG_INFO("internal coalesce internal is %d internal_plus is %d", internal->GetPageId(),
                 internal_plus->GetPageId());
        #endif
        if (!is_i_plus_before_i) {
          std::swap(internal, internal_plus);
        }
        if (!internal_plus->SetPairAt(internal_plus->GetSize(), {key_plus, internal->ValueAt(0)})) {
          LOG_DEBUG("coalesce: set pair failed");
        }

        auto child =
            reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(internal->ValueAt(0))->GetData());
        child->SetParentPageId(internal_plus->GetPageId());
        if (!buffer_pool_manager_->UnpinPage(child->GetPageId(), true)) {
          LOG_DEBUG("coalesce: unpin page failed");
        }

        internal_plus->IncreaseSize(1);
        // move pair
        for (int i = 1; i < internal->GetSize(); i++) {
          if (!internal_plus->SetPairAt(internal_plus->GetSize(), {internal->KeyAt(i), internal->ValueAt(i)})) {
            LOG_DEBUG("coalesce: set pair failed1");
          }
          auto child =
              reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(internal->ValueAt(i))->GetData());
          child->SetParentPageId(internal_plus->GetPageId());
          if (!buffer_pool_manager_->UnpinPage(child->GetPageId(), true)) {
            LOG_DEBUG("coalesce: unpin page failed");
          }
          internal_plus->IncreaseSize(1);
        }
        if (transaction != nullptr) {
          transaction->AddIntoDeletedPageSet(internal->GetPageId());
          // unlatch node1
          #ifdef PrintThreadInfo
          LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->back()->WUnlatch();
          #ifdef PrintThreadInfo
          LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                   std::hash<std::thread::id>{}(transaction->GetThreadId()));
          #endif
          page_set->pop_back();
        }
        BPlusTree::RemoveEntry(reinterpret_cast<BPlusTreePage *>(parent), key_plus, transaction);
        buffer_pool_manager_->DeletePage(internal->GetPageId());
        if (!buffer_pool_manager_->UnpinPage(internal_plus->GetPageId(), true)) {
          LOG_DEBUG("coalesce: unpin page failed");
        }
        return;
      }
      // #redistribute
      else {
        #ifdef PrintLogInfo
        LOG_INFO("internal redistribute internal is %d internal_plus is %d", internal->GetPageId(),
                 internal_plus->GetPageId());
        #endif
        KeyType temp_key;
        int temp_value = 0;
        int insert_idx = 0;
        if (is_i_plus_before_i) {
          temp_key = internal_plus->KeyAt(internal_plus->GetSize() - 1);
          temp_value = internal_plus->ValueAt(internal_plus->GetSize() - 1);
          // delete last key and value in internal_plus
          internal_plus->IncreaseSize(-1);
        } else {
          temp_key = internal_plus->KeyAt(1);
          temp_value = internal_plus->ValueAt(0);
          // delete first key and value in internal_plus
          internal_plus->SetValueAt(0, internal_plus->ValueAt(1));
          if (!internal_plus->DeletePair(temp_key, comparator_)) {
            LOG_DEBUG("redistribute: delete pair failed");
          }
          insert_idx = internal->GetSize();
        }

        auto child = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(temp_value)->GetData());
        child->SetParentPageId(internal->GetPageId());
        if (!buffer_pool_manager_->UnpinPage(child->GetPageId(), true)) {
          LOG_DEBUG("coalesce: unpin page failed");
        }

        #ifdef PrintLogInfo
        LOG_INFO("temp_value=%d", temp_value);
        #endif
        if (!internal->SetPairAt(insert_idx, {key_plus, temp_value})) {
          LOG_DEBUG("redistribute: set pair failed");
        }
        internal->IncreaseSize(1);

        for (int i = 1; i < parent->GetSize(); i++) {
          if (comparator_(parent->KeyAt(i), key_plus) == 0) {
            parent->SetKeyAt(i, temp_key);
            break;
          }
        }
        // unlatch
        if (transaction != nullptr) {
          for (auto p : *page_set) {
            #ifdef PrintThreadInfo
            LOG_INFO("T WUnlatch p%d thread %zu", p->GetPageId(),
                     std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
            p->WUnlatch();
            #ifdef PrintThreadInfo
            LOG_INFO("S WUnlatch p%d thread %zu", p->GetPageId(),
                     std::hash<std::thread::id>{}(transaction->GetThreadId()));
            #endif
            if (p->GetPageId() == root_page_id_) {
              #ifdef PrintRootInfo
              LOG_INFO("T unlock root id %d thread %zu", root_page_id_,
                       std::hash<std::thread::id>{}(transaction->GetThreadId()));
              #endif
              root_page_id_latch_.WUnlock();
              #ifdef PrintRootInfo
              LOG_INFO("S unlock root id %d thread %zu", root_page_id_,
                       std::hash<std::thread::id>{}(transaction->GetThreadId()));
              #endif
            }
          }
          page_set->clear();
        }

        if (!buffer_pool_manager_->UnpinPage(parent->GetPageId(), true)) {
          LOG_DEBUG("redistribute: unpin page failed");
        }
        if (!buffer_pool_manager_->UnpinPage(internal_plus->GetPageId(), true)) {
          LOG_DEBUG("redistribute: unpin page failed");
        }
        if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), true)) {
          LOG_DEBUG("redistribute: unpin page failed");
        }
      }
    }
    else {
      if (transaction != nullptr) {
        #ifdef PrintThreadInfo
        LOG_INFO("T WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->back()->WUnlatch();
        #ifdef PrintThreadInfo
        LOG_INFO("S WUnlatch p%d thread %zu", page_set->back()->GetPageId(),
                 std::hash<std::thread::id>{}(transaction->GetThreadId()));
        #endif
        page_set->pop_back();
      }
      if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), true)) {
        LOG_DEBUG("internal: unpin page failed");
      }
    }
  }
}

/*****************************************************************************
 * INDEX ITERATOR
 *****************************************************************************/
/*
 * Input parameter is void, find the leaftmost leaf page first, then construct
 * index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin() -> INDEXITERATOR_TYPE {
  auto node = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData());
  while (!node->IsLeafPage()) {
    auto internal = reinterpret_cast<InternalPage *>(node);
    auto first_child_page_id = internal->ValueAt(0);
    node = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(first_child_page_id)->GetData());
    if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), false)) {
      LOG_DEBUG("Begin: Unpin page failed");
    }
  }
  auto leaf = reinterpret_cast<LeafPage *>(node);
  return INDEXITERATOR_TYPE(leaf, 0, buffer_pool_manager_);
}

/*
 * Input parameter is low key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin(const KeyType &key) -> INDEXITERATOR_TYPE {
  int temp = false;
  int *index = &temp;
  Page *root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  LeafPage *leaf = BPlusTree::GetLeaf(key, index, OperateType::Iterator,nullptr, root_page);
  return INDEXITERATOR_TYPE(leaf, *index, buffer_pool_manager_);
}

/*
 * Input parameter is void, construct an index iterator representing the end
 * of the key/value pair in the leaf node
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::End() -> INDEXITERATOR_TYPE {
  auto node = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData());
  while (!node->IsLeafPage()) {
    auto internal = reinterpret_cast<InternalPage *>(node);
    int last_index = internal->GetSize() - 1 < 0 ? 0 : internal->GetSize() - 1;
    auto last_child_page_id = internal->ValueAt(last_index);
    node = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(last_child_page_id)->GetData());
    if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), false)) {
      LOG_DEBUG("Begin: Unpin page failed");
    }
  }
  auto leaf = reinterpret_cast<LeafPage *>(node);
  return INDEXITERATOR_TYPE(leaf, leaf->GetSize(), buffer_pool_manager_);
}

/**
 * @return Page id of the root of this tree
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetRootPageId() -> page_id_t { 
  return root_page_id_; }

/*****************************************************************************
 * UTILITIES AND DEBUG
 *****************************************************************************/
/*
 * Update/Insert root page id in header page(where page_id = 0, header_page is
 * defined under include/page/header_page.h)
 * Call this method everytime root page id is changed.
 * @parameter: insert_record      defualt value is false. When set to true,
 * insert a record <index_name, root_page_id> into header page instead of
 * updating it.
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::UpdateRootPageId(int insert_record) {
  auto *header_page = static_cast<HeaderPage *>(buffer_pool_manager_->FetchPage(HEADER_PAGE_ID));
  if (insert_record != 0) {
    // create a new record<index_name + root_page_id> in header_page
    header_page->InsertRecord(index_name_, root_page_id_);
  } else {
    // update root_page_id in header_page
    header_page->UpdateRecord(index_name_, root_page_id_);
  }
  buffer_pool_manager_->UnpinPage(HEADER_PAGE_ID, true);
}

/*
 * This method is used for test only
 * Read data from file and insert one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertFromFile(const std::string &file_name, Transaction *transaction) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;

    KeyType index_key;
    index_key.SetFromInteger(key);
    RID rid(key);
    BPlusTree::Insert(index_key, rid, transaction);
  }
}
/*
 * This method is used for test only
 * Read data from file and remove one by one
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::RemoveFromFile(const std::string &file_name, Transaction *transaction) {
  int64_t key;
  std::ifstream input(file_name);
  while (input) {
    input >> key;
    KeyType index_key;
    index_key.SetFromInteger(key);
    BPlusTree::Remove(index_key, transaction);
  }
}

/**
 * This method is used for debug only, You don't need to modify
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Draw(BufferPoolManager *buffer_pool_manager_, const std::string &outf) {
  if (IsEmpty()) {
    LOG_WARN("Draw an empty tree");
    return;
  }
  std::ofstream out(outf);
  out << "digraph G {" << std::endl;
  ToGraph(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData()),
          buffer_pool_manager_, out);
  out << "}" << std::endl;
  out.flush();
  out.close();
}

/**
 * This method is used for debug only, You don't need to modify
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Print(BufferPoolManager *buffer_pool_manager_) {
  if (IsEmpty()) {
    LOG_WARN("Print an empty tree");
    return;
  }
  ToString(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData()),
           buffer_pool_manager_);
}

/**
 * This method is used for debug only, You don't need to modify
 * @tparam KeyType
 * @tparam ValueType
 * @tparam KeyComparator
 * @param page
 * @param buffer_pool_manager_
 * @param out
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ToGraph(BPlusTreePage *page, BufferPoolManager *buffer_pool_manager_, std::ofstream &out) const {
  std::string leaf_prefix("LEAF_");
  std::string internal_prefix("INT_");
  if (page->IsLeafPage()) {
    auto *leaf = reinterpret_cast<LeafPage *>(page);
    // Print node name
    out << leaf_prefix << leaf->GetPageId();
    // Print node properties
    out << "[shape=plain color=green ";
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">P=" << leaf->GetPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << leaf->GetSize() << "\">"
        << "max_size=" << leaf->GetMaxSize() << ",min_size=" << leaf->GetMinSize() << ",size=" << leaf->GetSize()
        << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < leaf->GetSize(); i++) {
      out << "<TD>" << leaf->KeyAt(i) << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Leaf node link if there is a next page
    if (leaf->GetNextPageId() != INVALID_PAGE_ID) {
      out << leaf_prefix << leaf->GetPageId() << " -> " << leaf_prefix << leaf->GetNextPageId() << ";\n";
      out << "{rank=same " << leaf_prefix << leaf->GetPageId() << " " << leaf_prefix << leaf->GetNextPageId() << "};\n";
    }

    // Print parent links if there is a parent
    if (leaf->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << leaf->GetParentPageId() << ":p" << leaf->GetPageId() << " -> " << leaf_prefix
          << leaf->GetPageId() << ";\n";
    }
  } else {
    auto *inner = reinterpret_cast<InternalPage *>(page);
    // Print node name
    out << internal_prefix << inner->GetPageId();
    // Print node properties
    out << "[shape=plain color=pink ";  // why not?
    // Print data of the node
    out << "label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
    // Print data
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">P=" << inner->GetPageId() << "</TD></TR>\n";
    out << "<TR><TD COLSPAN=\"" << inner->GetSize() << "\">"
        << "max_size=" << inner->GetMaxSize() << ",min_size=" << inner->GetMinSize() << ",size=" << inner->GetSize()
        << "</TD></TR>\n";
    out << "<TR>";
    for (int i = 0; i < inner->GetSize(); i++) {
      out << "<TD PORT=\"p" << inner->ValueAt(i) << "\">";
      if (i > 0) {
        out << inner->KeyAt(i);
      } else {
        out << " ";
      }
      out << "</TD>\n";
    }
    out << "</TR>";
    // Print table end
    out << "</TABLE>>];\n";
    // Print Parent link
    if (inner->GetParentPageId() != INVALID_PAGE_ID) {
      out << internal_prefix << inner->GetParentPageId() << ":p" << inner->GetPageId() << " -> " << internal_prefix
          << inner->GetPageId() << ";\n";
    }
    // Print leaves
    for (int i = 0; i < inner->GetSize(); i++) {
      auto child_page =
          reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(inner->ValueAt(i))->GetData());
      ToGraph(child_page, buffer_pool_manager_, out);
      if (i > 0) {
        auto sibling_page =
            reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(inner->ValueAt(i - 1))->GetData());
        if (!sibling_page->IsLeafPage() && !child_page->IsLeafPage()) {
          out << "{rank=same " << internal_prefix << sibling_page->GetPageId() << " " << internal_prefix
              << child_page->GetPageId() << "};\n";
        }
        buffer_pool_manager_->UnpinPage(sibling_page->GetPageId(), false);
      }
    }
  }
  buffer_pool_manager_->UnpinPage(page->GetPageId(), false);
}

/**
 * This function is for debug only, you don't need to modify
 * @tparam KeyType
 * @tparam ValueType
 * @tparam KeyComparator
 * @param page
 * @param buffer_pool_manager_
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ToString(BPlusTreePage *page, BufferPoolManager *buffer_pool_manager_) const {
  if (page->IsLeafPage()) {
    auto *leaf = reinterpret_cast<LeafPage *>(page);
    std::cout << "Leaf Page: " << leaf->GetPageId() << " parent: " << leaf->GetParentPageId()
              << " next: " << leaf->GetNextPageId() << std::endl;
    for (int i = 0; i < leaf->GetSize(); i++) {
      std::cout << leaf->KeyAt(i) << ",";
    }
    std::cout << std::endl;
    std::cout << std::endl;
  } else {
    auto *internal = reinterpret_cast<InternalPage *>(page);
    std::cout << "Internal Page: " << internal->GetPageId() << " parent: " << internal->GetParentPageId() << std::endl;
    for (int i = 0; i < internal->GetSize(); i++) {
      std::cout << internal->KeyAt(i) << ": " << internal->ValueAt(i) << ",";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < internal->GetSize(); i++) {
      ToString(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(internal->ValueAt(i))->GetData()),
               buffer_pool_manager_);
    }
  }
  buffer_pool_manager_->UnpinPage(page->GetPageId(), false);
}

template class BPlusTree<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTree<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTree<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTree<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub