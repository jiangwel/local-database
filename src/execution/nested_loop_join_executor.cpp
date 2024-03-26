//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_loop_join_executor.cpp
//
// Identification: src/execution/nested_loop_join_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/nested_loop_join_executor.h"
#include "binder/table_ref/bound_join_ref.h"
#include "common/exception.h"

namespace bustub {
#define DEBUG
NestedLoopJoinExecutor::NestedLoopJoinExecutor(ExecutorContext *exec_ctx, const NestedLoopJoinPlanNode *plan,
                                               std::unique_ptr<AbstractExecutor> &&left_executor,
                                               std::unique_ptr<AbstractExecutor> &&right_executor)
    : AbstractExecutor(exec_ctx),
      plan_(plan),
      left_executor_(std::move(left_executor)),
      right_executor_(std::move(right_executor)) {
  if (!(plan->GetJoinType() == JoinType::LEFT || plan->GetJoinType() == JoinType::INNER)) {
    // Note for 2022 Fall: You ONLY need to implement left join and inner join.
    throw bustub::NotImplementedException(fmt::format("join type {} not supported", plan->GetJoinType()));
  }
}

void NestedLoopJoinExecutor::Init() {
  left_executor_->Init();
  right_executor_->Init();

  RID left_rid{};
  left_tuple_valid_ = left_executor_->Next(&left_tuple_, &left_rid);
}

auto NestedLoopJoinExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  Tuple right_tuple{};
  RID right_rid{};
  RID left_rid{};
  std::vector<Value> values{};
  values.reserve(GetOutputSchema().GetColumnCount());

  while (left_tuple_valid_) {
    auto right_tuple_valid = right_executor_->Next(&right_tuple, &right_rid);
    if (!right_tuple_valid) {
      // reset right executor
      right_executor_->Init();
      if (is_not_matched_ && ProcessLeftJoin(values, tuple)) {
        left_tuple_valid_ = left_executor_->Next(&left_tuple_, &left_rid);
        return true;
      }
      left_tuple_valid_ = left_executor_->Next(&left_tuple_, &left_rid);
      is_not_matched_ = true;
    } else {
      auto result = plan_->Predicate().EvaluateJoin(&left_tuple_, left_executor_->GetOutputSchema(), &right_tuple,
                                                    right_executor_->GetOutputSchema());
      if (ProcessJoinResult(result, &right_tuple, values, tuple)) {
        is_not_matched_ = false;
        return true;
      }
    }
  }

  return false;
}

auto NestedLoopJoinExecutor::ProcessLeftJoin(std::vector<Value> &values, Tuple *tuple) -> bool {
  if (plan_->GetJoinType() == JoinType::LEFT) {
    auto left_result = plan_->Predicate().Evaluate(&left_tuple_, left_executor_->GetOutputSchema());
    if (!left_result.IsNull()) {
      AddTupleValuesToVector(&left_tuple_, left_executor_->GetOutputSchema(), values);
      for (uint32_t i = 0; i < right_executor_->GetOutputSchema().GetColumnCount(); i++) {
        values.emplace_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
      }
      *tuple = Tuple{values, &GetOutputSchema()};
      return true;
    }
  }
  return false;
}

auto NestedLoopJoinExecutor::ProcessJoinResult(const Value &result, const Tuple *right_tuple,
                                               std::vector<Value> &values, Tuple *tuple) -> bool {
  switch (plan_->GetJoinType()) {
    case JoinType::INNER:
    case JoinType::LEFT:
      if (!result.IsNull() && result.GetAs<bool>()) {
        AddTupleValuesToVector(&left_tuple_, left_executor_->GetOutputSchema(), values);
        AddTupleValuesToVector(right_tuple, right_executor_->GetOutputSchema(), values);
        *tuple = Tuple{values, &GetOutputSchema()};
        return true;
      }
      break;
    default:
      break;
  }
  return false;
}
void NestedLoopJoinExecutor::AddTupleValuesToVector(const Tuple *tuple, const Schema &schema,
                                                    std::vector<Value> &values) {
  for (uint32_t i = 0; i < schema.GetColumnCount(); i++) {
    values.emplace_back(tuple->GetValue(&schema, i));
  }
}
}  // namespace bustub
