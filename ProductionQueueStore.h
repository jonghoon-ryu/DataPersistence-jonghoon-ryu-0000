#pragma once

#include <string>

#include "Json.h"
#include "JsonStore.h"

// FIFO production queue, persisted under jsonData/productionQueue.
//
// Entries are always appended (create) and only ever consumed from the
// front (dequeue), so the underlying insertion-ordered array doubles as a
// first-in-first-out queue: orders can be checked in the order they came in.
class ProductionQueueStore : public JsonStore
{
public:
    ProductionQueueStore();

    // Builds a queue entry: orderId, sampleName, orderQuantity, stock,
    // totalProductionTimeMin, estimatedCompletionTime.
    static JsonValue makeQueueItem(const std::string& orderId, const std::string& sampleName, int orderQuantity,
        int stock, double totalProductionTimeMin, const std::string& estimatedCompletionTime);

    // Returns the oldest (first-in) entry, or nullptr if the queue is empty.
    const JsonValue* front() const;

    // Removes the oldest (first-in) entry. Returns false if the queue is
    // empty. On success, outOrderId is set to the removed entry's order id.
    bool dequeue(std::string& outOrderId);
};
