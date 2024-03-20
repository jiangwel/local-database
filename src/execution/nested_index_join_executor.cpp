//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_index_join_executor.cpp
//
// Identification: src/execution/nested_index_join_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/nested_index_join_executor.h"

namespace bustub {

NestIndexJoinExecutor::NestIndexJoinExecutor(ExecutorContext *exec_ctx, const NestedIndexJoinPlanNode *plan,
                                             std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)){
  if (!(plan->GetJoinType() == JoinType::LEFT || plan->GetJoinType() == JoinType::INNER)) {
    // Note for 2022 Fall: You ONLY need to implement left join and inner join.
    throw bustub::NotImplementedException(fmt::format("join type {} not supported", plan->GetJoinType()));
  }
}

void NestIndexJoinExecutor::Init() {
  child_executor_->Init();
  auto index_info = exec_ctx_->GetCatalog()->GetIndex(plan_->GetIndexOid());
  index_tree_ = dynamic_cast<BPlusTreeIndexForOneIntegerColumn *>(index_info->index_.get());
  auto table_info = exec_ctx_->GetCatalog()->GetTable(plan_->GetInnerTableOid());
  table_heap_ = table_info->table_.get();
  RID left_rid{};
  left_tuple_valid_ = child_executor_->Next(&left_tuple_, &left_rid);
}

auto NestIndexJoinExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  RID left_rid{};
  std::vector<Value> values{};
  values.reserve(GetOutputSchema().GetColumnCount());
  while(left_tuple_valid_){
    auto key_value  = plan_->KeyPredicate().get()->Evaluate(&left_tuple_, child_executor_->GetOutputSchema());
    std::vector<Value> key_values{key_value};
    Tuple key_tuple(key_values,&plan_->InnerTableSchema());
    std::vector<bustub::RID> *scan_result;
    index_tree_->ScanKey(key_tuple,scan_result,exec_ctx_->GetTransaction());

    if(scan_result->empty()){
      if(plan_->GetJoinType() == JoinType::LEFT){
        AddTupleValuesToVector(&left_tuple_,GetOutputSchema(),values);
        AddTupleValuesToVector(nullptr,GetOutputSchema(),values);
        *tuple = Tuple(values,&GetOutputSchema());
        left_tuple_valid_ = child_executor_->Next(&left_tuple_, &left_rid);
        return true;
      }
      left_tuple_valid_ = child_executor_->Next(&left_tuple_, &left_rid);
    } else {
      Tuple* right_tuple_ptr=nullptr;
      table_heap_->GetTuple(scan_result->at(0),right_tuple_ptr,exec_ctx_->GetTransaction());
      auto result = plan_->KeyPredicate().get()->EvaluateJoin(&left_tuple_, GetOutputSchema(), right_tuple_ptr,
                                                    plan_->InnerTableSchema());
      if(ProcessJoinResult(result,right_tuple_ptr,values,tuple)){
        left_tuple_valid_ = child_executor_->Next(&left_tuple_, &left_rid);
        return true;
      }
      left_tuple_valid_ = child_executor_->Next(&left_tuple_, &left_rid);
    }
  }
  return false;
}
void NestIndexJoinExecutor::AddTupleValuesToVector(const Tuple *tuple, const Schema &schema,
                                                    std::vector<Value> &values) {
  if(tuple == nullptr){
    for (uint32_t i = 0; i < schema.GetColumnCount(); i++) {
      values.emplace_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
    }
    return;
  }
  for (uint32_t i = 0; i < schema.GetColumnCount(); i++) {
    values.emplace_back(tuple->GetValue(&schema, i));
  }
}
auto NestIndexJoinExecutor::ProcessJoinResult(const Value &result,const Tuple *right_tuple,
                                               std::vector<Value> &values, Tuple *tuple) -> bool {
  switch (plan_->GetJoinType()) {
    case JoinType::INNER:
    case JoinType::LEFT:
      if (!result.IsNull() && result.GetAs<bool>()) {
        AddTupleValuesToVector(&left_tuple_, GetOutputSchema(), values);
        AddTupleValuesToVector(right_tuple, GetOutputSchema(), values);
        *tuple = Tuple{values, &GetOutputSchema()};
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}

}  // namespace bustub
