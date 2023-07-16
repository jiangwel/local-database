#include <string>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "storage/index/b_plus_tree.h"
#include "storage/page/header_page.h"

namespace bustub {
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                          int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      root_page_id_(INVALID_PAGE_ID),
      buffer_pool_manager_(buffer_pool_manager),
      comparator_(comparator),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size) {}

/*
 * Helper function to decide whether current b+tree is empty
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::IsEmpty() const -> bool { return true; }
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
  bool temp = false;
  bool* is_repeat=&temp;
  LeafPage* leaf=GetLeaf(key,is_repeat);
  if(*is_repeat){
    for(int i=0;i<leaf->GetSize();i++){
      if(comparator_(leaf->KeyAt(i),key)==0){
        result->push_back(leaf->ValueAt(i));
        return true;
      }
    }
    return false;
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
  // not exist root
  if(root_page_id_==INVALID_PAGE_ID){
    // LOG_INFO("Insert: root page is null");
    auto root_page = buffer_pool_manager_->NewPage(&root_page_id_);
    
    auto root = reinterpret_cast<LeafPage*>(root_page->GetData());
    root->Init(root_page_id_,INVALID_PAGE_ID,leaf_max_size_);
    InsertNode(reinterpret_cast<BPlusTreePage*>(root),key,value);
    UpdateRootPageId(0);

    if(!buffer_pool_manager_->UnpinPage(root_page_id_,true)){
      LOG_DEBUG("Insert: unpin root page failed");
    }
    return true;
  }

  // Get leaf1
  bool temp = false;
  bool* is_repeat=&temp;
  LeafPage* leaf1=GetLeaf(key,is_repeat);
  if(*is_repeat){
    // Duplicate key, return false
    if(!buffer_pool_manager_->UnpinPage(leaf1->GetPageId(),false)){
      LOG_DEBUG("Insert: unpin leaf1 page failed");
    }
    if(!buffer_pool_manager_->UnpinPage(root_page_id_,false)){
      LOG_DEBUG("Insert: unpin root page failed");
    }
    return false;
  }
  
  auto page1 = reinterpret_cast<BPlusTreePage *>(leaf1);
  // LOG_INFO("size of page1 is: %d",page1->GetSize());
  
  // page1 is not full, insert directly
  if(page1->GetSize()<leaf_max_size_){
    InsertNode(page1,key,value);
    if(!buffer_pool_manager_->UnpinPage(root_page_id_,false)){
      LOG_DEBUG("Insert: unpin root page failed");
    }
    if(!buffer_pool_manager_->UnpinPage(page1->GetPageId(),true)){
      LOG_DEBUG("Insert: unpin leaf1 page failed");
    }
    return true;
  }
  // LOG_INFO("leaf_max_size_ is %d",leaf_max_size_);
  // page1 is full Create a new leaf
  page_id_t leaf2_page_id;
  auto leaf2_page = buffer_pool_manager_->NewPage(&leaf2_page_id);
  auto leaf2 = reinterpret_cast<LeafPage*>(leaf2_page->GetData());
  leaf2->Init(leaf2_page_id,leaf1->GetParentPageId(),leaf_max_size_);
  auto page2 = reinterpret_cast<BPlusTreePage*>(leaf2);

  // Splite page1 and insert key value
  if(SplitTree(page1,page2,key)){
    InsertNode(page2,key,value);
  } else {
    InsertNode(page1,key,value);
  }

  KeyType first_key = leaf2->KeyAt(0);
  InsertParent(page1,page2,first_key);
  // LOG_INFO("Split key is: %ld size of page1 is %d,page2 is %d,parent id is:%d,root id: %d",key.ToString(),page1->GetSize(),page2->GetSize(),page1->GetParentPageId(),root_page_id_);

  if(!buffer_pool_manager_->UnpinPage(root_page_id_,false)){
    LOG_DEBUG("Insert: unpin root page failed");
  }
  if(!buffer_pool_manager_->UnpinPage(page1->GetPageId(),true)){
    LOG_DEBUG("Insert: unpin leaf1 page failed");
  }
  if(!buffer_pool_manager_->UnpinPage(page2->GetPageId(),true)){
    LOG_DEBUG("Insert: unpin leaf2 page failed");
  }
  return true;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertNode(BPlusTreePage *node, const KeyType &key, const ValueType &value){
  int value_int=value.GetSlotNum();
  if(node->IsLeafPage()){
    auto leaf = reinterpret_cast<LeafPage*>(node);
    // leaf is empty
    if(leaf->GetSize()==0){
      if(!leaf->SetPairAt(leaf->GetSize(),MappingType(key,value))){
        LOG_DEBUG("InsertNode: set pair failed 1");
      }
      leaf->IncreaseSize(1);
      return;
    }
    // key < first key in leaf
    if(comparator_(key,leaf->KeyAt(0))==-1){
      if(!leaf->SetPairAt(0,MappingType(key,value))){
        LOG_DEBUG("InsertNode: set pair failed 2");
      }
      leaf->IncreaseSize(1);
      return;
    }
    
    for(int i = 0;i<leaf->GetSize();i++){
      // key < it->first
      if(comparator_(key,leaf->KeyAt(i))==-1){
        if(!leaf->SetPairAt(i,MappingType(key,value))){
          LOG_DEBUG("InsertNode: set pair failed 3");
        }
        leaf->IncreaseSize(1);
        return;
      }
    }

    // key > last key in leaf
    if(!leaf->SetPairAt(leaf->GetSize(),MappingType(key,value))){
      LOG_DEBUG("InsertNode: set pair failed 6");
    }
    leaf->IncreaseSize(1);
    return;
  }
  
  auto internal = reinterpret_cast<InternalPage*>(node);
  // key < first key in internal
  if(comparator_(key,internal->KeyAt(0))==-1){
    if(!internal->SetPairAt(0,std::make_pair(key,value_int))){
      LOG_DEBUG("InsertNode: set pair failed 4");
    }
    internal->IncreaseSize(1);
    return;
  }
  // insert
  for(int i = 0;i<internal->GetSize();i++){
    // key < it->first
    if(comparator_(key,internal->KeyAt(i))==-1){
      if(!internal->SetPairAt(i,std::make_pair(key,value_int))){
        LOG_DEBUG("InsertNode: set pair failed 5");
      }
      internal->IncreaseSize(1);
      return;
    }
  }
  // key > last key in internal
  if(!internal->SetPairAt(internal->GetSize(),std::make_pair(key,value_int))){
    LOG_DEBUG("InsertNode: set pair failed 7");
  }
  internal->IncreaseSize(1);
  return;
}

INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetLeaf(const KeyType &key,bool* is_repeat)->LeafPage*{
  auto root_page = buffer_pool_manager_->FetchPage(root_page_id_);
  auto node = reinterpret_cast<BPlusTreePage*>(root_page->GetData());

  while(!node->IsLeafPage()){
    bool found = false;
    auto node_as_internal = reinterpret_cast<InternalPage *>(node);
    page_id_t next_page_id;

    // find key <= it->first
    for(int i = 0;i<node_as_internal->GetSize();i++){
      if(comparator_(key,node_as_internal->KeyAt(i))!=1){
        if(comparator_(key,node_as_internal->KeyAt(i))==0){
          next_page_id = node_as_internal->ValueAt(i+1);
        } else {
          next_page_id = node_as_internal->ValueAt(i);
        }
        found = true;
        break;
      }
    }
    // not exist key <= it->first
    if(!found){
      next_page_id = node_as_internal->ValueAt(node_as_internal->GetSize()-1);
      // LOG_INFO("GetLeaf: not found next page id is %d this page id is:%d",next_page_id,node_as_internal->GetPageId());
    }
    if(!buffer_pool_manager_->UnpinPage(node_as_internal->GetPageId(),false)){
      LOG_DEBUG("GetLeaf: unpin internal page failed");
    }
    node = reinterpret_cast<BPlusTreePage*>(buffer_pool_manager_->FetchPage(next_page_id)->GetData());
  }

  auto leaf = reinterpret_cast<LeafPage *>(node);
  // check if key exists in leaf
  // LOG_INFO("fetched page size is: %d",leaf->GetSize());
  // LOG_INFO("size is %d %p next id: %d",leaf->GetSize(),leaf,leaf->GetNextPageId());
  for(int i = 0;i<leaf->GetSize();i++){
    if(comparator_(key,leaf->KeyAt(i))==0){
      *is_repeat = true;
      return leaf;
    }
  }

  // LOG_INFO("GetLeaf: leaf size id is %p",leaf);
  *is_repeat = false;
  return leaf;
}

INDEX_TEMPLATE_ARGUMENTS
bool BPLUSTREE_TYPE::SplitTree(BPlusTreePage *page1, BPlusTreePage *page2, const KeyType &key){
  int half_index=0;
  if(page1->IsLeafPage()){
    auto leaf1 = reinterpret_cast<LeafPage*>(page1);
    auto leaf2 = reinterpret_cast<LeafPage*>(page2);
    leaf2->SetNextPageId(leaf1->GetNextPageId());
    leaf1->SetNextPageId(leaf2->GetPageId());
    // Rounded up
    half_index = (leaf_max_size_+1)/2;

    // k>leaf1[mid].key 
    if(comparator_(key,leaf1->KeyAt(half_index))==1){
      // leaf1[mid+1,end]=>leaf2
      for(int i=half_index+1;i<leaf1->GetSize();++i){
        leaf2->SetPairAt(i,{leaf1->KeyAt(i),leaf1->ValueAt(i)});
      }
      int increase_size = leaf1->GetSize()-(half_index+1);
      leaf2->IncreaseSize(increase_size);
      leaf1->IncreaseSize(-increase_size);
      return true;
    }
    // leaf1[mid,end]=>leaf2
    for(int i=half_index;i<leaf1->GetSize();++i){
      leaf2->SetPairAt(i,{leaf1->KeyAt(i),leaf1->ValueAt(i)});
    }
    int increase_size = leaf1->GetSize()-half_index;
    leaf2->IncreaseSize(increase_size);
    leaf1->IncreaseSize(-increase_size);
    return false;
  }
  // page1,2 is internal page
  auto internal1 = reinterpret_cast<InternalPage*>(page1);
  auto internal2 = reinterpret_cast<InternalPage*>(page2);
  half_index = (internal_max_size_+1)/2;

  // k>internal1[mid].key 
  if(comparator_(key,internal1->KeyAt(half_index))==1){
    // internal1[mid-1,end]=>internal2
    for(int i=half_index-1;i<internal1->GetSize();++i){
      internal2->SetPairAt(i,{internal1->KeyAt(i),internal1->ValueAt(i)});
    }
    int increase_size = internal1->GetSize()-(half_index-1);
    internal2->IncreaseSize(increase_size);
    internal1->IncreaseSize(-increase_size);
    return true;
  }
  // internal1[mid,end]=>internal2
  for(int i=half_index;i<internal1->GetSize();++i){
    internal2->SetPairAt(i,{internal1->KeyAt(i),internal1->ValueAt(i)});
  }
  int increase_size = internal1->GetSize()-half_index;
  internal2->IncreaseSize(increase_size);
  internal1->IncreaseSize(-increase_size);
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertParent(BPlusTreePage *page1, BPlusTreePage *page2, const KeyType &key){
  if(page1->IsRootPage()){
    // Create a new root
    page_id_t root_page_id;
    auto new_root_page = buffer_pool_manager_->NewPage(&root_page_id);
    auto new_root = reinterpret_cast<InternalPage*>(new_root_page->GetData());
    new_root->Init(root_page_id);

    // Insert page1,key,page2 into new root
    KeyType invalid_key;
    if(!new_root->SetPairAt(0,std::make_pair(invalid_key,page1->GetPageId()))){
      LOG_DEBUG("InsertParent: set pair failed 1");
    }
    new_root->IncreaseSize(1);
    if(!new_root->SetPairAt(1,std::make_pair(key,page2->GetPageId()))){
      LOG_DEBUG("InsertParent: set pair failed 2");
    }
    // LOG_INFO("page2 id is %d",page2->GetPageId());
    new_root->IncreaseSize(1);

    page1->SetParentPageId(root_page_id);
    page2->SetParentPageId(root_page_id);
    root_page_id_ = root_page_id;
    UpdateRootPageId(1);
    if(!buffer_pool_manager_->UnpinPage(root_page_id,true)){
      LOG_DEBUG("InsertParent: unpin root page failed");
    }
    // LOG_INFO("create a new root");
    return;
  }
  
  auto parent = reinterpret_cast<BPlusTreePage*>(buffer_pool_manager_->FetchPage(page1->GetParentPageId())->GetData());
  if(parent->GetSize()==internal_max_size_){
    // create a new page
    page_id_t parent_page_prime_id;
    auto parent_page_prime = buffer_pool_manager_->NewPage(&parent_page_prime_id);
    auto parent_prime = reinterpret_cast<InternalPage*>(parent_page_prime->GetData());
    parent_prime->Init(parent_page_prime_id,parent->GetParentPageId());

    // split parent
    RID rid;
    if(SplitTree(parent,reinterpret_cast<BPlusTreePage*>(parent_prime),key)){
      rid.Set(parent->GetPageId(),parent->GetPageId() & 0xFFFFFFFF);
      InsertNode(parent_prime,key,rid);
    } else {
      rid.Set(parent_prime->GetPageId(),parent_prime->GetPageId() & 0xFFFFFFFF);
      InsertNode(parent,key,rid);
    }
    auto parent_internal = reinterpret_cast<InternalPage*>(parent);
    auto k_prime = parent_internal->KeyAt(parent_internal->GetSize()-1);
    InsertParent(parent,parent_prime,k_prime);
    if(!buffer_pool_manager_->UnpinPage(parent_page_prime_id,true)){
      LOG_DEBUG("InsertParent: Unpin page failed");
    }
    if(!buffer_pool_manager_->UnpinPage(page1->GetParentPageId(),true)){
      LOG_DEBUG("InsertParent: Unpin page failed");
    }
    return;
  }

  RID rid(page2->GetPageId(),page2->GetPageId() & 0xFFFFFFFF);
  InsertNode(parent,key,rid);
  if(!buffer_pool_manager_->UnpinPage(page1->GetParentPageId(),true)){
    LOG_DEBUG("InsertParent: Unpin page failed");
  }
  return;
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
void BPLUSTREE_TYPE::Remove(const KeyType &key, Transaction *transaction) {}

/*****************************************************************************
 * INDEX ITERATOR
 *****************************************************************************/
/*
 * Input parameter is void, find the leaftmost leaf page first, then construct
 * index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin() -> INDEXITERATOR_TYPE { return INDEXITERATOR_TYPE(); }

/*
 * Input parameter is low key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin(const KeyType &key) -> INDEXITERATOR_TYPE { return INDEXITERATOR_TYPE(); }

/*
 * Input parameter is void, construct an index iterator representing the end
 * of the key/value pair in the leaf node
 * @return : index iterator
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::End() -> INDEXITERATOR_TYPE { return INDEXITERATOR_TYPE(); }

/**
 * @return Page id of the root of this tree
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetRootPageId() -> page_id_t { return root_page_id_; }

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
    Insert(index_key, rid, transaction);
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
    Remove(index_key, transaction);
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
  ToGraph(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData()), buffer_pool_manager_, out);
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
  ToString(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(root_page_id_)->GetData()), buffer_pool_manager_);
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
      auto child_page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(inner->ValueAt(i))->GetData());
      ToGraph(child_page, buffer_pool_manager_, out);
      if (i > 0) {
        auto sibling_page = reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(inner->ValueAt(i - 1))->GetData());
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
      ToString(reinterpret_cast<BPlusTreePage *>(buffer_pool_manager_->FetchPage(internal->ValueAt(i))->GetData()), buffer_pool_manager_);
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
