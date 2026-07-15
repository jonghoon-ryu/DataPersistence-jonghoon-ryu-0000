#include "gtest/gtest.h"

#include <filesystem>
#include <regex>

#include "Json.h"
#include "JsonStore.h"
#include "OrderStore.h"
#include "ProductionQueueStore.h"
#include "SampleStore.h"
#include "TimeUtil.h"

TEST(SampleTest, BasicAssertion) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(JsonStoreTest, CreateAndRead) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("1");
    record["name"] = JsonValue("Alice");
    record["address"] = JsonValue("Seoul");

    store.create(record);

    const JsonValue* found = store.read("1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->at("name").asString(), "Alice");
    EXPECT_EQ(found->at("address").asString(), "Seoul");
    EXPECT_EQ(store.size(), 1u);
}

TEST(JsonStoreTest, CreateDuplicateIdThrows) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("1");
    store.create(record);

    EXPECT_THROW(store.create(record), std::invalid_argument);
}

TEST(JsonStoreTest, ReadMissingReturnsNull) {
    JsonStore store;
    EXPECT_EQ(store.read("missing"), nullptr);
}

TEST(JsonStoreTest, Update) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("1");
    record["name"] = JsonValue("Alice");
    record["address"] = JsonValue("Seoul");
    store.create(record);

    JsonValue updated = JsonValue::makeObject();
    updated["name"] = JsonValue("Bob");
    updated["address"] = JsonValue("Busan");
    EXPECT_TRUE(store.update("1", updated));

    const JsonValue* found = store.read("1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->at("name").asString(), "Bob");
    EXPECT_EQ(found->at("address").asString(), "Busan");
    EXPECT_EQ(found->at("id").asString(), "1");
}

TEST(JsonStoreTest, UpdateMissingReturnsFalse) {
    JsonStore store;
    EXPECT_FALSE(store.update("missing", JsonValue::makeObject()));
}

TEST(JsonStoreTest, Delete) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("1");
    store.create(record);

    EXPECT_TRUE(store.remove("1"));
    EXPECT_EQ(store.read("1"), nullptr);
    EXPECT_EQ(store.size(), 0u);
}

TEST(JsonStoreTest, DeleteMissingReturnsFalse) {
    JsonStore store;
    EXPECT_FALSE(store.remove("missing"));
}

TEST(JsonStoreTest, ParseFromJsonText) {
    JsonStore store = JsonStore::parse(
        R"([{"id": "1", "name": "Alice", "address": "Seoul"}, {"id": "2", "name": "Bob", "address": "Busan"}])");

    EXPECT_EQ(store.size(), 2u);
    const JsonValue* first = store.read("1");
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first->at("name").asString(), "Alice");
    EXPECT_EQ(first->at("address").asString(), "Seoul");
}

TEST(JsonStorePersistenceTest, CreateWritesFileToJsonDataDirectory) {
    JsonStore store;  // default persist directory: "jsonData"
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("persist-1");
    record["name"] = JsonValue("Carol");

    store.create(record);

    std::filesystem::path filePath = std::filesystem::path(store.persistDirectory()) / "persist-1.json";
    EXPECT_EQ(store.persistDirectory(), "jsonData");
    EXPECT_TRUE(std::filesystem::exists(filePath));

    std::filesystem::remove(filePath);
}

TEST(JsonStorePersistenceTest, UpdateRewritesPersistedFile) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("persist-2");
    record["name"] = JsonValue("Carol");
    store.create(record);

    JsonValue updated = JsonValue::makeObject();
    updated["name"] = JsonValue("Dave");
    store.update("persist-2", updated);

    std::filesystem::path filePath = std::filesystem::path(store.persistDirectory()) / "persist-2.json";
    JsonValue persisted = JsonValue::parseFile(filePath.string());
    EXPECT_EQ(persisted.at("name").asString(), "Dave");

    std::filesystem::remove(filePath);
}

TEST(JsonStorePersistenceTest, DeleteRemovesPersistedFile) {
    JsonStore store;
    JsonValue record = JsonValue::makeObject();
    record["id"] = JsonValue("persist-3");
    store.create(record);

    std::filesystem::path filePath = std::filesystem::path(store.persistDirectory()) / "persist-3.json";
    ASSERT_TRUE(std::filesystem::exists(filePath));

    store.remove("persist-3");

    EXPECT_FALSE(std::filesystem::exists(filePath));
}

TEST(TimeUtilTest, ReturnsSecondPrecisionFormattedTimestamp) {
    std::string timestamp = getCurrentTimestamp();

    static const std::regex pattern(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    EXPECT_TRUE(std::regex_match(timestamp, pattern)) << "got: " << timestamp;
}

TEST(SampleStoreTest, CreateAndPersistToSamplesDirectory) {
    SampleStore store;
    EXPECT_EQ(store.persistDirectory(), "jsonData/samples");

    JsonValue sample = SampleStore::makeSample("S-001", "실리콘 웨이퍼-8인치", 0.5, 0.92, 480);
    store.create(sample);

    const JsonValue* found = store.read("S-001");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->at("name").asString(), "실리콘 웨이퍼-8인치");
    EXPECT_EQ(found->at("stock").asNumber(), 480);

    std::filesystem::path filePath = std::filesystem::path(store.persistDirectory()) / "S-001.json";
    EXPECT_TRUE(std::filesystem::exists(filePath));

    store.remove("S-001");
}

TEST(OrderStoreTest, CreateOrderGeneratesFormattedOrderNumberAndPersistsToOrdersDirectory) {
    OrderStore store;
    EXPECT_EQ(store.persistDirectory(), "jsonData/orders");

    std::string orderNumber = store.createOrder("S-001", "삼성전자 파운드리", 200);

    static const std::regex pattern(R"(^\d{4}-\d{2}-\d{2}-\d{2}-\d{2}-\d{2}-S-001-\d{3}$)");
    EXPECT_TRUE(std::regex_match(orderNumber, pattern)) << "got: " << orderNumber;

    const JsonValue* found = store.read(orderNumber);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->at("sampleId").asString(), "S-001");
    EXPECT_EQ(found->at("customerName").asString(), "삼성전자 파운드리");
    EXPECT_EQ(found->at("quantity").asNumber(), 200);
    EXPECT_EQ(found->at("status").asString(), "RESERVED");

    store.remove(orderNumber);
}

TEST(OrderStoreTest, SequenceIncrementsPerOrder) {
    OrderStore store;
    std::string first = store.createOrder("S-002", "LG이노텍", 100);
    std::string second = store.createOrder("S-002", "LG이노텍", 50);

    EXPECT_NE(first, second);
    EXPECT_NE(first.find("-000"), std::string::npos);
    EXPECT_NE(second.find("-001"), std::string::npos);

    store.remove(first);
    store.remove(second);
}

TEST(ProductionQueueStoreTest, FifoOrderIsPreserved) {
    ProductionQueueStore queue;
    EXPECT_EQ(queue.persistDirectory(), "jsonData/productionQueue");

    queue.create(ProductionQueueStore::makeQueueItem("ORD-1", "산화막 웨이퍼-SiO2", 150, 0, 190.0, "11:43"));
    queue.create(ProductionQueueStore::makeQueueItem("ORD-2", "SiC 파워기판-6인치", 200, 30, 165.0, "14:28"));
    queue.create(ProductionQueueStore::makeQueueItem("ORD-3", "GaN 에피택셜-4인치", 300, 220, 34.0, "15:02"));

    ASSERT_NE(queue.front(), nullptr);
    EXPECT_EQ(queue.front()->at("orderId").asString(), "ORD-1");

    std::string dequeued;
    ASSERT_TRUE(queue.dequeue(dequeued));
    EXPECT_EQ(dequeued, "ORD-1");
    ASSERT_NE(queue.front(), nullptr);
    EXPECT_EQ(queue.front()->at("orderId").asString(), "ORD-2");

    ASSERT_TRUE(queue.dequeue(dequeued));
    EXPECT_EQ(dequeued, "ORD-2");
    ASSERT_TRUE(queue.dequeue(dequeued));
    EXPECT_EQ(dequeued, "ORD-3");

    EXPECT_EQ(queue.front(), nullptr);
    std::string empty;
    EXPECT_FALSE(queue.dequeue(empty));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
