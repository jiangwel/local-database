/**
 * index_iterator.cpp
 */
#include <cassert>

#include "storage/index/index_iterator.h"
#include "common/logger.h"

namespace bustub {

/*
 * NOTE: you can change the destructor/constructor method here
 * set your own input parameters
 */
INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::IndexIterator(LeafPage *leaf,int index,BufferPoolManager *bpm):leaf_(leaf),index_(index),bpm_(bpm){};

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE::~IndexIterator(){
    if(!bpm_->UnpinPage(leaf_->GetPageId(),false)){
        LOG_DEBUG("unpin page failed");
    }
};  // NOLINT

INDEX_TEMPLATE_ARGUMENTS
auto INDEXITERATOR_TYPE::IsEnd() -> bool { return leaf_->GetNextPageId()==INVALID_PAGE_ID&&index_==leaf_->GetSize()-1; }

INDEX_TEMPLATE_ARGUMENTS
auto INDEXITERATOR_TYPE::operator*() -> const MappingType & { return MappingType(leaf_->KeyAt(index_),leaf_->ValueAt(index)); }

INDEX_TEMPLATE_ARGUMENTS
auto INDEXITERATOR_TYPE::operator++() -> INDEXITERATOR_TYPE & { 
    auto next_page_id=leaf_->GetNextPageId();
    if(index+1==leaf->GetSize()){
        if(next_page_id==INVALID_PAGE_ID){
            if(i==leaf_->GetSize()){
                LOG_DEBUG("index out of range");
                return;
            }
            index_++;
            return *this;
        }
        if(!bpm_->UnpinPage(leaf_->GetPageId(),false)){
            LOG_DEBUG("unpin page failed");
        }
        leaf_=reinterpret_cast<LeafPage *>(bpm_->FetchPage(next_page_id)->GetData());
        index_=0;
        return *this;
    }
    index_++;
    return *this;
}

template class IndexIterator<GenericKey<4>, RID, GenericComparator<4>>;

template class IndexIterator<GenericKey<8>, RID, GenericComparator<8>>;

template class IndexIterator<GenericKey<16>, RID, GenericComparator<16>>;

template class IndexIterator<GenericKey<32>, RID, GenericComparator<32>>;

template class IndexIterator<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
