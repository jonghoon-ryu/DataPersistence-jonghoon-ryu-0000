#include "SampleStore.h"

SampleStore::SampleStore() : JsonStore(std::string("jsonData/samples")) {}

JsonValue SampleStore::makeSample(
    const std::string& id, const std::string& name, double avgProductionTimeMin, double yieldRate, int stock)
{
    JsonValue sample = JsonValue::makeObject();
    sample["id"] = JsonValue(id);
    sample["name"] = JsonValue(name);
    sample["avgProductionTimeMin"] = JsonValue(avgProductionTimeMin);
    sample["yieldRate"] = JsonValue(yieldRate);
    sample["stock"] = JsonValue(stock);
    return sample;
}
