//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_executor.cpp
//
// Identification: src/execution/delete_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>

#include "execution/executors/delete_executor.h"

namespace bustub {

DeleteExecutor::DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void DeleteExecutor::Init() {
  child_executor_->Init();
  table_info_ = GetExecutorContext()->GetCatalog()->GetTable(plan_->TableOid());
  table_heap_ = table_info_->table_.get();
  table_indexs_ = GetExecutorContext()->GetCatalog()->GetTableIndexes(table_info_->name_);
}

void DeleteExecutor::UpdateIndex(Tuple *tuple, RID *rid) {
  if (table_indexs_.empty()) {
    return;
  }
  for (auto index_info : table_indexs_) {
    auto index = index_info->index_.get();
    auto key = tuple->KeyFromTuple(table_info_->schema_, *index->GetKeySchema(), index->GetKeyAttrs());
    index->DeleteEntry(key, *rid, GetExecutorContext()->GetTransaction());
  }
}

auto DeleteExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (done_flag_) {
    return false;
  }
  done_flag_ = true;

  Tuple child_tuple{};
  int deleted_rows_num = 0;

  // Insert tuples
  while (child_executor_->Next(&child_tuple, rid)) {
    deleted_rows_num++;
    table_heap_->MarkDelete(*rid, GetExecutorContext()->GetTransaction());
    UpdateIndex(&child_tuple, rid);
  }

  std::vector<Value> values{};
  values.reserve(1);
  values.push_back(ValueFactory::GetIntegerValue(deleted_rows_num));
  *tuple = Tuple{values, &GetOutputSchema()};

  return true;
}

}  // namespace bustub
