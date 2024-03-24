#include "execution/executors/sort_executor.h"

namespace bustub {

SortExecutor::SortExecutor(ExecutorContext *exec_ctx, const SortPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx),plan_(plan),child_executor_(std::move(child_executor)) {}

void SortExecutor::Init() {
    child_executor_->Init();
    Tuple tuple{}; 
    RID rid{};
    while(child_executor_->Next(&tuple, &rid)){
        sort_tuples_.push_back(tuple);
    }
    SortComparator comparator(plan_);
    std::sort(sort_tuples_.begin(), sort_tuples_.end(), comparator);
}

auto SortExecutor::Next(Tuple *tuple, RID *rid) -> bool {
    if(cursor_ >= sort_tuples_.size()){
        cursor_ = 0;
        sort_tuples_.clear();
        child_executor_->Init();
        return false;
    }
    *tuple = sort_tuples_[cursor_];
    cursor_++;
    return true;
}

inline auto SortExecutor::SortComparator::operator()(const Tuple& ltuple,const Tuple& rtuple) const->bool{
    auto &order_bys = plan_->GetOrderBy();
    for(size_t i=0;i<order_bys.size();i++){
        auto &order_by_type = order_bys[i].first;
        auto &order_by_expr = order_bys[i].second;
        auto lvalue = order_by_expr->Evaluate(&ltuple, plan_->OutputSchema());
        auto rvalue = order_by_expr->Evaluate(&rtuple, plan_->OutputSchema());

        if(lvalue.CompareEquals(rvalue) == CmpBool::CmpTrue){
            continue;
        }

        bool is_asc = order_by_type == OrderByType::ASC || order_by_type == OrderByType::DEFAULT || order_by_type == OrderByType::INVALID;
        auto result = is_asc ? lvalue.CompareLessThan(rvalue) : lvalue.CompareGreaterThan(rvalue);
        return result == CmpBool::CmpTrue;
    }
    return false;
}

}  // namespace bustub
