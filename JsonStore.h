#pragma once

#include <string>

#include "Json.h"

// A minimal CRUD store on top of JsonValue. Records are stored as a JSON
// array of objects; each record must carry a unique string "id" field.
class JsonStore
{
public:
    JsonStore();
    explicit JsonStore(JsonValue records);

    static JsonStore parse(const std::string& text);
    static JsonStore loadFromFile(const std::string& path);
    void saveToFile(const std::string& path) const;
    std::string serialize() const;

    // Create: record must contain a unique "id" (string) field.
    // Throws std::invalid_argument if "id" is missing/not a string,
    // or if a record with that id already exists.
    void create(const JsonValue& record);

    // Read: returns nullptr if no record with the given id exists.
    const JsonValue* read(const std::string& id) const;

    // Update: replaces the record with the given id (the "id" field is
    // forced to stay the same). Returns false if the id doesn't exist.
    bool update(const std::string& id, JsonValue record);

    // Delete: removes the record with the given id. Returns false if the
    // id doesn't exist.
    bool remove(const std::string& id);

    size_t size() const;
    const JsonValue& records() const { return records_; }

private:
    int indexOf(const std::string& id) const;

    JsonValue records_;
};
