#include "ProductionQueueStore.h"

ProductionQueueStore::ProductionQueueStore() : JsonStore(std::string("jsonData/productionQueue")) {}

JsonValue ProductionQueueStore::makeQueueItem(const std::string& orderId, const std::string& sampleName,
    int orderQuantity, int stock, double totalProductionTimeMin, const std::string& estimatedCompletionTime)
{
    JsonValue item = JsonValue::makeObject();
    item["id"] = JsonValue(orderId);
    item["orderId"] = JsonValue(orderId);
    item["sampleName"] = JsonValue(sampleName);
    item["orderQuantity"] = JsonValue(orderQuantity);
    item["stock"] = JsonValue(stock);
    item["totalProductionTimeMin"] = JsonValue(totalProductionTimeMin);
    item["estimatedCompletionTime"] = JsonValue(estimatedCompletionTime);
    return item;
}

const JsonValue* ProductionQueueStore::front() const
{
    const auto& array = records().asArray();
    if (array.empty())
    {
        return nullptr;
    }
    return &array.front();
}

bool ProductionQueueStore::dequeue(std::string& outOrderId)
{
    const JsonValue* first = front();
    if (!first)
    {
        return false;
    }

    outOrderId = first->at("id").asString();
    return remove(outOrderId);
}
