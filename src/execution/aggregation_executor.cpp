//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.cpp
//
// Identification: src/execution/aggregation_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>
#include <vector>

#include "execution/executors/aggregation_executor.h"

namespace bustub {

AggregationExecutor::AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                                         std::unique_ptr<AbstractExecutor> &&child)
    : AbstractExecutor(exec_ctx),
      plan_(plan),
      child_(std::move(child)),
      aht_(SimpleAggregationHashTable(plan_->GetAggregates(), plan_->GetAggregateTypes())),
      aht_iterator_(SimpleAggregationHashTable::Iterator(aht_.Begin())) {}

void AggregationExecutor::Init() {
  child_->Init();
  Tuple child_tuple{};
  RID rid{};
  std::vector<Value> null_value(5, ValueFactory::GetNullValueByType(TypeId::INTEGER));
  AggregateValue null_agg_value = {null_value};
  AggregateKey null_agg_key = {};

  bool is_child_empty = !child_->Next(&child_tuple, &rid);
  if (is_child_empty) {
    if (plan_->GetGroupBys().empty()) {
      aht_.InsertCombine(null_agg_key, null_agg_value);
    }
    aht_iterator_ = aht_.Begin();
    return;
  }

  do {
    aht_.InsertCombine(MakeAggregateKey(&child_tuple), MakeAggregateValue(&child_tuple));
  } while (child_->Next(&child_tuple, &rid));

  aht_iterator_ = aht_.Begin();
}

auto AggregationExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (aht_iterator_ == aht_.End()) {
    aht_.Clear();
    return false;
  }

  std::vector<Value> values{};
  auto values_size = GetOutputSchema().GetColumnCount();
  values.reserve(values_size);

  for (size_t i = 0; i < aht_iterator_.Key().group_bys_.size(); i++) {
    values.emplace_back(aht_iterator_.Key().group_bys_[i]);
  }
  for (size_t i = 0; i < aht_iterator_.Val().aggregates_.size(); i++) {
    values.emplace_back(aht_iterator_.Val().aggregates_[i]);
  }
  ++aht_iterator_;

  *tuple = Tuple{values, &GetOutputSchema()};
  return true;
}

auto AggregationExecutor::GetChildExecutor() const -> const AbstractExecutor * { return child_.get(); }

}  // namespace bustub
