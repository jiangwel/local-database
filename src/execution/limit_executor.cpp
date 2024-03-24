//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.cpp
//
// Identification: src/execution/limit_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/limit_executor.h"

namespace bustub {

LimitExecutor::LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                             std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx),plan_(plan),child_executor_(std::move(child_executor)) {}

void LimitExecutor::Init() {
    child_executor_->Init();
    Tuple tuple{}; 
    RID rid{};
    tuples_.reserve(plan_->GetLimit());
    while(child_executor_->Next(&tuple, &rid)){
        tuples_.emplace_back(tuple);
        if(tuples_.size() == plan_->GetLimit()){
            break;
        }
    }
}

auto LimitExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    if(cursor_ >= plan_->GetLimit() || cursor_ >= tuples_.size()){
        cursor_ = 0;
        return false;
    }
    *tuple = tuples_[cursor_++];
    return true;
}

}  // namespace bustub
