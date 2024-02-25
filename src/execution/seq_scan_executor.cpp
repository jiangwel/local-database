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

SeqScanExecutor::SeqScanExecutor(ExecutorContext *exec_ctx, const SeqScanPlanNode *plan) : AbstractExecutor(exec_ctx) {}

void SeqScanExecutor::Init() { throw NotImplementedException("SeqScanExecutor is not implemented"); }

auto SeqScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    auto* exec_ctx = this->GetExecutorContext();
    auto* catalog = exec_ctx->GetCatalog();
    auto table_oid = this->plan_->GetTableOid();
    auto* table_metadata = catalog->GetTable(table_oid);
    if(table_metadata == Catalog::NULL_TABLE_INFO){
        return false;
    }
    auto* table = table_metadata->table_.get();
    auto it = table->Begin(exec_ctx->GetTransaction());
    if(it == table->End()){
        return false;
    }
    *tuple = *it;
    *rid = it->GetRid();
    return true;
}

}  // namespace bustub
