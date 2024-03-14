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
    : AbstractExecutor(exec_ctx),plan_(plan),child_(std::move(child)),aht_(SimpleAggregationHashTable(plan_->GetAggregates(),plan_->GetAggregateTypes())),aht_iterator_(SimpleAggregationHashTable::Iterator(aht_.Begin())) {}

void AggregationExecutor::Init() {
    child_->Init();
}

auto AggregationExecutor::Next(Tuple *tuple, RID *rid) -> bool { 
    if(done_flag_){
        return false;
    }
    done_flag_ = true;

    Tuple child_tuple{};
    std::vector<Value> null_value{ValueFactory::GetNullValueByType(TypeId::INTEGER),ValueFactory::GetNullValueByType(TypeId::INTEGER),ValueFactory::GetNullValueByType(TypeId::INTEGER),ValueFactory::GetNullValueByType(TypeId::INTEGER),ValueFactory::GetNullValueByType(TypeId::INTEGER)};
    AggregateValue null_agg_value = {null_value};
    bool is_child_empty = !child_->Next(&child_tuple, rid);

    if(is_child_empty){
        aht_.InsertCombine(MakeAggregateKey(&child_tuple), null_agg_value);
    } else {
        do{
            aht_.InsertCombine(MakeAggregateKey(&child_tuple), MakeAggregateValue(&child_tuple));
        } while(child_->Next(&child_tuple, rid));
    }

    

    aht_iterator_ = aht_.Begin();
    std::vector<Value> values{};
    auto valus_size = plan_->GetAggregates().size();
    values.reserve(valus_size);

    for(;aht_iterator_!=aht_.End();++aht_iterator_){
        for(auto value:aht_iterator_.Val().aggregates_){
            values.emplace_back(value);
        }
    }

    *tuple = Tuple{values, &GetOutputSchema()};
    return true; 
}

auto AggregationExecutor::GetChildExecutor() const -> const AbstractExecutor * { return child_.get(); }

}  // namespace bustub
