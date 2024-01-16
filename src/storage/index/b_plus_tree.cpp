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
  //LOG_INFO("page--");
  root_page->RLatch();
  if(transaction != nullptr){
    transaction->GetPageSet()->push_back(root_page);
  }
  root_page_id_latch_.RUnlock();

  auto[leaf,index] = GetLeaf(key, OperateType::Find, transaction, root_page);
  bool is_key_found = index != -1;
  // key is found
  if (is_key_found) {
    result->push_back(leaf->ValueAt(index));
    if(!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false)){
      LOG_DEBUG("GetValue: unpin leaf page failed");
    }
    return true;
  }
  if(!buffer_pool_manager_->UnpinPage(leaf->GetPageId(), false)){
      LOG_DEBUG("GetValue: unpin leaf page failed");
  }
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ReleaseResourcesd(Transaction *transaction){
  for(auto p : *transaction->GetPageSet()){
    p->WUnlatch();
    if (!buffer_pool_manager_->UnpinPage(p->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin page failed");
    }
    //LOG_INFO("page++");
  }
  transaction->GetPageSet()->clear();
  for (auto p : *transaction->GetDeletedPageSet()) {
    buffer_pool_manager_->DeletePage(p);
  }
  transaction->GetDeletedPageSet()->clear();
}
/*****************************************************************************
 * INSERTION
 *****************************************************************************/

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::MakeRoot(const KeyType &key, const ValueType &value){
  page_id_t new_root_page_id;
  auto new_root = reinterpret_cast<LeafPage *>(buffer_pool_manager_->NewPage(&new_root_page_id)->GetData());
  root_page_id_ = new_root_page_id;
  new_root->Init(root_page_id_, INVALID_PAGE_ID, leaf_max_size_);
  InsertNode(reinterpret_cast<BPlusTreePage *>(new_root), key, value);
  UpdateRootPageId(0);
  buffer_pool_manager_->UnpinPage(new_root->GetPageId(), true);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertInFillNode(LeafPage *leaf1,const KeyType &key, const ValueType &value,Transaction *transaction){
  page_id_t new_node_id;
  auto new_page = buffer_pool_manager_->NewPage(&new_node_id);
  new_page->WLatch();
  auto page_set = transaction->GetPageSet();
  page_set->push_back(new_page);

  auto leaf2 = reinterpret_cast<LeafPage *>(new_page->GetData());
  leaf2->Init(new_node_id, leaf1->GetParentPageId(), leaf_max_size_);

  // splite
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
  auto node2 = reinterpret_cast<BPlusTreePage *>(leaf2);
  auto node1 = reinterpret_cast<BPlusTreePage *>(leaf1);
  BPlusTree::InsertParent(node1, node2, first_key, rid, transaction);
}


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
  auto key_value = key.ToString();
  LOG_INFO("@Insert key is: %ld rid.GetPageId:%d thread %zu", key_value, value.GetPageId(),
           std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif

  if (IsEmpty()) {
    MakeRoot(key,value);
    root_page_id_latch_.WUnlock();
    return true;
  }

  auto page_set = transaction->GetPageSet();
  auto root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  root_page->WLatch();
  page_set->push_back(root_page);
  auto [leaf1,index] = GetLeaf(key, OperateType::Insert, transaction, root_page);
  bool leaf1IsFull = leaf1->GetSize() +1 == leaf_max_size_;
  bool is_key_found = index!=-1;

  if (is_key_found) {
    root_page_id_latch_.WUnlock();
    ReleaseResourcesd(transaction); 
    return false;
  }

  auto node1 = reinterpret_cast<BPlusTreePage *>(leaf1);

  if (!leaf1IsFull) {
    root_page_id_latch_.WUnlock();
    InsertNode(node1, key, value);
    ReleaseResourcesd(transaction);
    return true;
  }

  InsertNode(node1, key, value);
  InsertInFillNode(leaf1,key,value,transaction);
  ReleaseResourcesd(transaction);
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
auto BPLUSTREE_TYPE::GetLeaf(const KeyType &key, OperateType operator_type, Transaction *transaction,Page* node_page)
    -> std::tuple<LeafPage *, int> {
  #ifdef PrintLogInfo
  LOG_INFO("GetLeaf");
  #endif
  auto node = reinterpret_cast<BPlusTreePage *>(node_page->GetData());
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
    if(operator_type == OperateType::Find){
      node_page->RUnlatch();
      if(!buffer_pool_manager_->UnpinPage(node_page->GetPageId(), false)){
        LOG_DEBUG("unpin page failed");
      }
    }
    // all key > input_kay
    next_page_id = !found ? (node_as_internal->ValueAt(node_as_internal->GetSize() - 1)) : next_page_id;
    node_page = buffer_pool_manager_->BufferPoolManager::FetchPage(next_page_id);
    //LOG_INFO("page--");
    node = reinterpret_cast<BPlusTreePage *>(node_page->GetData());
    if(operator_type == OperateType::Find){ 
      node_page->RLatch();
    } else { 
      LockAndUnlock(node_page, node, operator_type, transaction);
    }
  }

  auto leaf = reinterpret_cast<LeafPage *>(node);
  // check if key exists in leaf
  for (int i = 0; i < leaf->GetSize(); i++) {
    if (comparator_(key, leaf->KeyAt(i)) == 0) {
      return {leaf,i};
    }
  }

  return {leaf,-1};
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
      if (!buffer_pool_manager_->UnpinPage(p->GetPageId(), true)) {
        LOG_DEBUG("Insert: unpin page failed");
      }
      //LOG_INFO("page++");
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
  auto page_set = transaction->GetPageSet();

  if (page1->IsRootPage()) {
    RenewRoot(page1,page2,key);
    return;
  }

  auto *parent = reinterpret_cast<InternalPage *>(buffer_pool_manager_->BufferPoolManager::FetchPage(page1->GetParentPageId())->GetData());
  bool is_parent_full = parent->GetSize() == parent->GetMaxSize();

  if (is_parent_full) {
    InsertInFillParent(page1,page2,parent,key,value,transaction);
    buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
    return;
  }
  root_page_id_latch_.WUnlock();
  InsertNode(parent, key, value);
  buffer_pool_manager_->UnpinPage(parent->GetPageId(), true);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::RenewRoot(BPlusTreePage *page1, BPlusTreePage *page2,const KeyType &key){
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
  root_page_id_ = root_page_id;
  UpdateRootPageId(1);
  root_page_id_latch_.WUnlock();
  buffer_pool_manager_->UnpinPage(new_root->GetPageId(), true);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertInFillParent(BPlusTreePage *page1, BPlusTreePage *page2,InternalPage * parent,const KeyType &key, const ValueType &value, Transaction *transaction){
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
    //LOG_INFO("page--");
    move_page->SetParentPageId(parent_prime->GetPageId());
    if (!buffer_pool_manager_->UnpinPage(move_page->GetPageId(), true)) {
      LOG_DEBUG("Unpin page failed");
    }
    //LOG_INFO("page++");
    // move to parent_prime
    parent_prime->SetPairAt(parent_prime->GetSize(), {parent->KeyAt(i), parent->ValueAt(i)});
    parent_prime->IncreaseSize(1);
  }
  int increase_size = parent->GetSize() - half_index;
  parent->IncreaseSize(-increase_size);
  // end split

  RID rid(parent_page_prime_id, parent_page_prime_id & 0xFFFFFFFF);
  
  InsertParent(parent, parent_prime, k_prime, rid, transaction);
  buffer_pool_manager_->UnpinPage(parent_prime->GetPageId(), true);
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
    //LOG_INFO("page--");
    root_page->WLatch();
    transaction->GetPageSet()->push_back(root_page);

    auto [leaf,index] = BPlusTree::GetLeaf(key, OperateType::Delete, transaction, root_page);
    bool leafIsRoot = leaf->IsRootPage(),leafSizeIs1 = leaf->GetSize() == 1,leafIsInternal = !leaf->IsLeafPage();
    bool is_key_found = index!=-1;
    if (!is_key_found) {
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
  auto page_set = transaction->GetPageSet();
  LeafPage *leaf = nullptr;
  InternalPage *internal = nullptr;
  if(node1->IsLeafPage()){
    leaf = reinterpret_cast<LeafPage *>(node1);
    leaf->DeletePair(key, comparator_);
  } else {
    internal = reinterpret_cast<InternalPage *>(node1);
    internal->DeletePair(key, comparator_);
  }
  bool is_node_one_child = node1->GetSize()==1;
  if(node1->IsRootPage()){
    if(!node1->IsLeafPage()&& is_node_one_child){
      ReplaceRootByChildren(reinterpret_cast<InternalPage *>(node1));
      transaction->AddIntoDeletedPageSet(node1->GetPageId());
      page_set->back()->WUnlatch();
      page_set->pop_back();
      root_page_id_latch_.WUnlock();
      buffer_pool_manager_->UnpinPage(node1->GetPageId(), true);
      buffer_pool_manager_->DeletePage(node1->GetPageId());
      return;
    }
    page_set->back()->WUnlatch();
    page_set->pop_back();
    root_page_id_latch_.WUnlock();
    return;
  }// #leaf
  if (node1->IsLeafPage()) {
    #ifdef PrintLogInfo
    LOG_INFO("node1 is leaf");
    #endif
    KeyType key_plus = leaf->KeyAt(0);
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
        // page_set->push_back(sibling_page);
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
            // page_set->push_back(sibling_page);
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
        // root_page_id_latch_.WUnlock();
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
          root_page_id_latch_.WUnlock();
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
        root_page_id_latch_.WUnlock();
      }
      if (!buffer_pool_manager_->UnpinPage(internal->GetPageId(), true)) {
        LOG_DEBUG("internal: unpin page failed");
      }
    }
  }
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ReplaceRootByChildren(InternalPage *old_root){
  auto new_root_id = old_root->ValueAt(0);
  auto new_root = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(new_root_id)->GetData());
  new_root->SetParentPageId(INVALID_PAGE_ID);
  root_page_id_ = new_root_id;
  UpdateRootPageId(1);
  buffer_pool_manager_->UnpinPage(new_root_id, true);
}

INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetSiblingIdx(InternalPage *parent_page,const int page_id)->int{
  int sibling_page_idx = -1;
  for (int i=0; i< parent_page->GetSize(); i++) {
    if (parent_page->ValueAt(i) == page_id) {
      sibling_page_idx = i==0?i+1:i-1;
      break;
    }
  } 
  return sibling_page_idx;
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
  Page *root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  auto [leaf,index] = BPlusTree::GetLeaf(key, OperateType::Iterator,nullptr, root_page);
  return INDEXITERATOR_TYPE(leaf, index, buffer_pool_manager_);
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