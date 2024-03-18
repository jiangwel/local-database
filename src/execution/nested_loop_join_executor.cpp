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

NestedLoopJoinExecutor::NestedLoopJoinExecutor(ExecutorContext *exec_ctx, const NestedLoopJoinPlanNode *plan,
                                               std::unique_ptr<AbstractExecutor> &&left_executor,
                                               std::unique_ptr<AbstractExecutor> &&right_executor)
    : AbstractExecutor(exec_ctx),plan_(plan),left_executor_(std::move(left_executor)),right_executor_(std::move(right_executor)) {
  if (!(plan->GetJoinType() == JoinType::LEFT || plan->GetJoinType() == JoinType::INNER)) {
    // Note for 2022 Fall: You ONLY need to implement left join and inner join.
    throw bustub::NotImplementedException(fmt::format("join type {} not supported", plan->GetJoinType()));
  }
}

void NestedLoopJoinExecutor::Init() {
  left_executor_->Init();
  right_executor_->Init();
}

auto NestedLoopJoinExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    auto nestedloopjoin_expr = plan_->Predicate();
    Tuple left_tuple{};
    Tuple right_tuple{};
    RID left_rid{};
    RID right_rid{};
    while(left_executor_->Next(&left_tuple, &left_rid)){
        while(right_executor_->Next(&right_tuple, &right_rid)){
          auto result = nestedloopjoin_expr.EvaluateJoin(&left_tuple, left_executor_->GetOutputSchema(), &right_tuple, right_executor_->GetOutputSchema());
          switch (plan_->GetJoinType()) {
            case JoinType::INNER:
                if (!result.IsNull() && result.GetAs<bool>()) {
                    *tuple = left_tuple;
                    *rid = left_rid;
                    return true;
                }
                break;
            case JoinType::LEFT:
                if(!result.IsNull()){
                    *tuple = left_tuple;
                    *rid = left_rid;
                    return true;
                }
                break;
          }
        }
        right_executor_->Init();
    }
    return false;
}

}  // namespace bustub
