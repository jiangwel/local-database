#include "execution/executors/sort_executor.h"

namespace bustub {

SortExecutor::SortExecutor(ExecutorContext *exec_ctx, const SortPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx),plan_(plan),child_executor_(std::move(child_executor)) {}

void SortExecutor::Init() {
    child_executor_->Init();
    Tuple* tuple_ptr = nullptr; 
    RID rid{};
    while(child_executor_->Next(tuple_ptr, &rid)){
        sort_tuples_.push_back(tuple_ptr);
    }
    auto &order_bys = plan_->GetOrderBy();
    SortComparator comparator(nullptr, plan_->OutputSchema());
    for(auto &order_by : order_bys){
        comparator.SetOrderBy(&order_by);
        std::sort(sort_tuples_.begin(), sort_tuples_.end(), comparator);
    }
}

auto SortExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    if(cursor_ >= sort_tuples_.size()){
        return false;
    }
    *tuple = *sort_tuples_[cursor_];
    cursor_++;
    return true;
}

inline auto SortExecutor::SortComparator::operator()(const Tuple & ltuple,const Tuple &rtuple) const->bool{
    auto &order_by_expr = order_by_->second;
    auto &order_by_type = order_by_->first;
    auto lvalue = order_by_expr->Evaluate(&ltuple, schema_);
    auto rvalue = order_by_expr->Evaluate(&rtuple, schema_);
    if(order_by_type == OrderByType::INVALID || 
        order_by_type == OrderByType::DEFAULT ||
        order_by_type == OrderByType::ASC){
       return lvalue.CompareLessThan(rvalue) == CmpBool::CmpTrue ? true : false; 
    } else {
        return lvalue.CompareGreaterThan(rvalue) == CmpBool::CmpTrue ? true : false;
    }
}
}  // namespace bustub
