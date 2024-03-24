#include "execution/executors/topn_executor.h"

namespace bustub {

TopNExecutor::TopNExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx),plan_(plan),child_executor_(std::move(child_executor)) {}

void TopNExecutor::Init() {}

auto TopNExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

inline auto TopNExecutor::TopNComparator::operator()(const Tuple& ltuple,const Tuple& rtuple) const->bool{
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
        auto result = is_asc ? lvalue.CompareGreaterThan(rvalue) : lvalue.CompareLessThan(rvalue);
        return result == CmpBool::CmpTrue;
    }
    return false;
}
}  // namespace bustub
