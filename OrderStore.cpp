#include "OrderStore.h"

#include <iomanip>
#include <sstream>

#include "TimeUtil.h"

OrderStore::OrderStore() : JsonStore(std::string("jsonData/orders")) {}

std::string OrderStore::generateOrderNumber(const std::string& sampleId, int sequence)
{
    std::ostringstream oss;
    oss << getCurrentTimestampDashed() << "-" << sampleId << "-" << std::setw(3) << std::setfill('0') << sequence;
    return oss.str();
}

std::string OrderStore::createOrder(
    const std::string& sampleId, const std::string& customerName, int quantity, const std::string& status)
{
    std::string orderNumber = generateOrderNumber(sampleId, nextSequence_++);

    JsonValue order = JsonValue::makeObject();
    order["id"] = JsonValue(orderNumber);
    order["sampleId"] = JsonValue(sampleId);
    order["customerName"] = JsonValue(customerName);
    order["quantity"] = JsonValue(quantity);
    order["status"] = JsonValue(status);

    create(order);
    return orderNumber;
}
