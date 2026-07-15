#include "gtest/gtest.h"

#include <filesystem>
#include <regex>

#include "Json.h"
#include "JsonStore.h"
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
