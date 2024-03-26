#include "optimizer/optimizer.h"
#include "execution/plans/sort_plan.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/topn_plan.h"
#include "execution/plans/projection_plan.h"
#include "common/logger.h"

namespace bustub {

auto Optimizer::OptimizeSortLimitAsTopN(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  std::vector<AbstractPlanNodeRef> children;
  for (const auto &child : plan->GetChildren()) {
    children.emplace_back(OptimizeSortLimitAsTopN(child));
  }
  auto optimized_plan = plan->CloneWithChildren(std::move(children));

  if(optimized_plan->GetType() == PlanType::Limit){
    const auto &limit_plan = dynamic_cast<const LimitPlanNode &>(*optimized_plan);
    BUSTUB_ENSURE(limit_plan.children_.size() == 1, "Limit should have exactly 1 children.");

    const auto &child_plan = optimized_plan->children_[0];
    if (child_plan->GetType() == PlanType::Sort) {
      const auto &sort_plan = dynamic_cast<const SortPlanNode &>(*child_plan);
      BUSTUB_ENSURE(child_plan->GetChildren().size() == 1, "Sort should have exactly 1 children.");

      return std::make_shared<TopNPlanNode>(limit_plan.output_schema_, sort_plan.GetChildPlan(),
                                            sort_plan.GetOrderBy(), limit_plan.GetLimit());
    }
  } 
  return optimized_plan;
}

}  // namespace bustub
