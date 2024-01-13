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

  int temp = -1;
  int *index = &temp;
  LeafPage *leaf = GetLeaf(key, index, OperateType::Find, transaction, root_page);
  // key is found
  if (*index != -1) {
    result->push_back(leaf->ValueAt(*index));
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
    //LOG_INFO("page--");
    root_page_id_ = new_root_page_id;
    root->Init(root_page_id_, INVALID_PAGE_ID, leaf_max_size_);
    InsertNode(reinterpret_cast<BPlusTreePage *>(root), key, value);
    UpdateRootPageId(0);

    if (!buffer_pool_manager_->UnpinPage(root->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin root page failed");
    }
    //LOG_INFO("page++");
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
  //LOG_INFO("page--");
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
    ReleaseResourcesd(transaction); 
    #ifdef PrintLogInfo
    LOG_INFO("Insert: duplicate key");
    #endif
    return false;
  }

  auto node1 = reinterpret_cast<BPlusTreePage *>(leaf1);

  // node1 is not full, insert directly
  if (!leaf1IsFull) {
    InsertNode(node1, key, value);
    ReleaseResourcesd(transaction);
    return true;
  }

  // node1 is full Create a new leaf
  page_id_t leaf2_page_id;
  #ifdef PrintThreadInfo
  LOG_INFO("T get leaf2 page thread %zu", std::hash<std::thread::id>{}(transaction->GetThreadId()));
  #endif
  auto leaf2_page = buffer_pool_manager_->NewPage(&leaf2_page_id);
  //LOG_INFO("page--");
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
auto BPLUSTREE_TYPE::GetLeaf(const KeyType &key, int *index, OperateType operator_type, Transaction *transaction,Page* node_page)
    -> LeafPage * {
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
  std::shared_ptr<std::deque<bustub::Page *>> page_set = nullptr;
  if (transaction != nullptr) {
    page_set = transaction->GetPageSet();
  }

  if (page1->IsRootPage()) {
    // Create a new root
    page_id_t root_page_id;
    auto new_root = reinterpret_cast<InternalPage *>(buffer_pool_manager_->NewPage(&root_page_id)->GetData());
    //LOG_INFO("page--");
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
    
    if (!buffer_pool_manager_->UnpinPage(new_root->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin new_root page failed");
    }
    //LOG_INFO("page++");
    return;
  }

  auto parent_page = buffer_pool_manager_->BufferPoolManager::FetchPage(page1->GetParentPageId());
  //LOG_INFO("page--");
  auto *parent = reinterpret_cast<InternalPage *>(parent_page->GetData());
  // parent is full
  if (parent->GetSize() == parent->GetMaxSize()) {
    #ifdef PrintLogInfo
    LOG_INFO("parent is full");
    #endif
    // create a new page
    page_id_t parent_page_prime_id;
    auto parent_prime_page = buffer_pool_manager_->NewPage(&parent_page_prime_id);
    //LOG_INFO("page--");
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
    if (!buffer_pool_manager_->UnpinPage(parent_prime->GetPageId(), true)) {
      LOG_DEBUG("Insert: unpin parent_prime page failed");
    }
    //LOG_INFO("page++");
    if(!buffer_pool_manager_->UnpinPage(parent->GetPageId(), true)){
      LOG_DEBUG("Insert: unpin parent page failed");
    }
    //LOG_INFO("page++");
    return;
  }
  root_page_id_latch_.WUnlock();
  // parent is not full
  InsertNode(parent, key, value);
  if(!buffer_pool_manager_->UnpinPage(parent->GetPageId(), true)){
    LOG_DEBUG("Insert: unpin parent page failed");
  }
  //LOG_INFO("page++");
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
  if (IsEmpty()) {
    root_page_id_latch_.WUnlock();
    return;
  }
    Page *root_page = buffer_pool_manager_->FetchPage(root_page_id_);
    //LOG_INFO("page--");
    root_page->WLatch();
    transaction->GetPageSet()->push_back(root_page);

    int temp = -1;
    int *index = &temp;
    LeafPage *leaf = BPlusTree::GetLeaf(key, index, OperateType::Delete, transaction, root_page);
    if (*index == -1) {
      ReleaseResourcesd(transaction);
    } else {
      BPlusTree::RemoveEntry(reinterpret_cast<BPlusTreePage *>(leaf), key, transaction);
    }
  }
}

INDEX_TEMPLATE_ARGUMENTS
template <typename P>
void BPLUSTREE_TYPE::RemoveEntry(P *bptree_page, const KeyType &key, Transaction *transaction) {
  #ifdef PrintLogInfo
  LOG_INFO("RemoveEntry: key=%ld thread: %zu node1: %d", key.ToString(),
           std::hash<std::thread::id>{}(transaction->GetThreadId()), node1->GetPageId());
  #endif
  bptree_page->DeletePair(key, comparator_);

  bool is_node_one_child = bptree_page->GetSize()==1;

  if(bptree_page->IsRootPage() && is_node_one_child){
    ReplaceRootByChildren();
  }

  bool is_key_num_not_enough = bptree_page->GetSize() < bptree_page->GetMinSize();  

  if(!is_key_num_not_enough){
    return;
  }
  auto *parent_page = reinterpret_cast<InternalPage *>(buffer_pool_manager_->FetchPage(bptree_page->GetParentPageId())->GetData());
  auto sibling_id = parent_page->GetSibling(bptree_page->GetPageId());
  auto sibling_page = reinterpret_cast<P *>(buffer_pool_manager_->FetchPage(sibling_id)->GetData());
  bool is_sibling_brother = sibling_page->GetNextPageId() == bptree_page->GetNextPageId();
  auto key_plus = parent_page->KeyAt(is_sibling_brother ? sibling_id + 1 : sib_index);
  bool able_to_coalesce = bptree_page->GetSize() + sibling_page->GetSize() <= bptree_page->GetMaxSize();

  if(able_to_coalesce){
    Coalesce(is_sibling_brother,bptree_page);
  } else {
    Redistribute();
  }
}
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::ReplaceRootByChildren(InternalPage *old_root){
  auto new_root_id = old_root->ValueAt(0);
  auto new_root = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(new_root_id->GetData());
  new_root->SetParentPageId(INVALID_PAGE_ID);
  root_page_id_ = new_root_id;
  UpdateRootPageId(1);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Coalesce(bool is_sibling_brother){
  if(!is_sibling_brother){
    std::swap(bptree_page, sibling_page);
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