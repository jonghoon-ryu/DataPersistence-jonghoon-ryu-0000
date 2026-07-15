#pragma once

#include <string>

#include "Json.h"
#include "JsonStore.h"

// CRUD store for 주문 (Order) records, persisted under jsonData/orders.
//
// Order numbers are generated as "YYYY-MM-DD-HH-MM-SS-<sampleId>-<seq>"
// (e.g. "2026-07-15-11-08-05-S-001-000"), where <seq> is a 3-digit,
// zero-padded counter that increments per order created by this store
// instance.
class OrderStore : public JsonStore
{
public:
    OrderStore();

    // Generates a unique order number, builds the order record (sampleId,
    // customerName, quantity, status), persists it, and returns the
    // generated order number.
    std::string createOrder(const std::string& sampleId, const std::string& customerName, int quantity,
        const std::string& status = "RESERVED");

    static std::string generateOrderNumber(const std::string& sampleId, int sequence);

private:
    int nextSequence_ = 0;
};
