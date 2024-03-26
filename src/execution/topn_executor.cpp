#include "execution/executors/topn_executor.h"

namespace bustub {

TopNExecutor::TopNExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void TopNExecutor::Init() {
  if (is_init_) {
    results_idx_ = results_.size() - 1;
    return;
  }
  child_executor_->Init();
  Tuple tuple{};
  RID rid{};
  TopNComparator comparator(plan_);
  std::priority_queue<Tuple, std::vector<Tuple>, TopNComparator> pq(comparator);

  while (child_executor_->Next(&tuple, &rid)) {
    if (pq.size() < plan_->GetN()) {
      pq.emplace(tuple);
    } else {
      if (comparator(tuple, pq.top())) {
        pq.pop();
        pq.emplace(tuple);
      }
    }
  }
  results_.reserve(plan_->GetN());
  while (!pq.empty()) {
    results_.emplace_back(pq.top());
    pq.pop();
  }
  results_idx_ = results_.size() - 1;
  is_init_ = true;
}

auto TopNExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (results_idx_ >= results_.size()) {
    return false;
  }
  *tuple = results_[results_idx_--];
  return true;
}

inline auto TopNExecutor::TopNComparator::operator()(const Tuple &ltuple, const Tuple &rtuple) const -> bool {
  auto &order_bys = plan_->GetOrderBy();
  for (const auto &order_by : order_bys) {
    auto &order_by_type = order_by.first;
    auto &order_by_expr = order_by.second;
    auto lvalue = order_by_expr->Evaluate(&ltuple, plan_->OutputSchema());
    auto rvalue = order_by_expr->Evaluate(&rtuple, plan_->OutputSchema());

    if (lvalue.CompareEquals(rvalue) == CmpBool::CmpTrue) {
      continue;
    }

    bool is_asc = order_by_type == OrderByType::ASC || order_by_type == OrderByType::DEFAULT ||
                  order_by_type == OrderByType::INVALID;
    auto result = is_asc ? lvalue.CompareLessThan(rvalue) : lvalue.CompareGreaterThan(rvalue);
    return result == CmpBool::CmpTrue;
  }
  return false;
}
}  // namespace bustub
