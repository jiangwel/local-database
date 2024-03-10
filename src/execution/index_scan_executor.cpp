//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_scan_executor.cpp
//
// Identification: src/execution/index_scan_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include "execution/executors/index_scan_executor.h"

namespace bustub {
IndexScanExecutor::IndexScanExecutor(ExecutorContext *exec_ctx, const IndexScanPlanNode *plan)
    : AbstractExecutor(exec_ctx),plan_(plan) {}

void IndexScanExecutor::Init() {
    auto* index_info = GetExecutorContext()->GetCatalog()->GetIndex(plan_->GetIndexOid());
    auto* tree = dynamic_cast<BPlusTreeIndexForOneIntegerColumn *>(index_info->index_.get());
    index_iter_ = tree->GetBeginIterator();
    table_heap_ = GetExecutorContext()->GetCatalog()->GetTable(index_info->table_name_)->table_.get();
}

auto IndexScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    if( index_iter_.IsInvaildIndexIter() || index_iter_.IsEnd()){
        return false;
    }
    if(!table_heap_->GetTuple((*index_iter_).second, tuple, exec_ctx_->GetTransaction())){
        return false;
    }
    ++index_iter_;
    return true;
}

}  // namespace bustub
