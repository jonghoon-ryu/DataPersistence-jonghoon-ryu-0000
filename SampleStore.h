#pragma once

#include <string>

#include "Json.h"
#include "JsonStore.h"

// CRUD store for 시료 (Sample) records, persisted under jsonData/samples.
class SampleStore : public JsonStore
{
public:
    SampleStore();

    // Builds a sample record: id, name, avgProductionTimeMin, yieldRate, stock.
    static JsonValue makeSample(
        const std::string& id, const std::string& name, double avgProductionTimeMin, double yieldRate, int stock);
};
