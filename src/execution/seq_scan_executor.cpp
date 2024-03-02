//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_executor.cpp
//
// Identification: src/execution/seq_scan_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/seq_scan_executor.h"

namespace bustub {

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx),plan_(plan){}

void SeqScanExecutor::Init() {
    auto* exec_ctx = GetExecutorContext();
    auto* catalog = exec_ctx->GetCatalog();
    auto table_oid = plan_->GetTableOid();
    auto* table_metadata = catalog->GetTable(table_oid);
    table_heap_ = table_metadata->table_.get();
    table_iter_ = table_heap_->Begin(exec_ctx->GetTransaction());
    PrintTableIndex();
}

auto SeqScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    if(table_iter_ == table_heap_->End()){
        return false;
    }
    *rid = table_iter_->GetRid();
    std::vector<Value> values{};
    auto value_num = GetOutputSchema().GetColumnCount();
    values.reserve(value_num);
    for(uint32_t i =0;i<value_num;i++){
        values.push_back(table_iter_->GetValue(&GetOutputSchema(),i));
    }
    *tuple = Tuple{values, &GetOutputSchema()};
    
    table_iter_++;
    return true;
}

void SeqScanExecutor::PrintTableIndex(){
    auto* exec_ctx = this->GetExecutorContext();
    auto* catalog = exec_ctx->GetCatalog();
    auto table_oid = plan_->GetTableOid();
    auto* table_metadata = catalog->GetTable(table_oid);
    auto table_index = catalog->GetTableIndexes(table_metadata->name_);
    for(auto index_info:table_index){
        auto* index = index_info->index_.get();
        LOG_INFO("Index name: %s",index->ToString().c_str());
    }

}
}  // namespace bustub
