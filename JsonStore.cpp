#include "JsonStore.h"

#include <stdexcept>

JsonStore::JsonStore() : records_(JsonValue::makeArray()) {}

JsonStore::JsonStore(JsonValue records) : records_(std::move(records))
{
    if (!records_.isArray())
    {
        throw std::invalid_argument("JsonStore records must be a JSON array");
    }
}

JsonStore JsonStore::parse(const std::string& text)
{
    return JsonStore(JsonValue::parse(text));
}

JsonStore JsonStore::loadFromFile(const std::string& path)
{
    return JsonStore(JsonValue::parseFile(path));
}

void JsonStore::saveToFile(const std::string& path) const
{
    records_.saveToFile(path);
}

std::string JsonStore::serialize() const
{
    return records_.serialize();
}

int JsonStore::indexOf(const std::string& id) const
{
    const auto& array = records_.asArray();
    for (size_t i = 0; i < array.size(); ++i)
    {
        if (array[i].has("id") && array[i].at("id").isString() && array[i].at("id").asString() == id)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void JsonStore::create(const JsonValue& record)
{
    if (!record.has("id") || !record.at("id").isString())
    {
        throw std::invalid_argument("record must have a string \"id\" field");
    }

    const std::string& id = record.at("id").asString();
    if (indexOf(id) != -1)
    {
        throw std::invalid_argument("record with id \"" + id + "\" already exists");
    }

    records_.asArray().push_back(record);
}

const JsonValue* JsonStore::read(const std::string& id) const
{
    int index = indexOf(id);
    if (index == -1)
    {
        return nullptr;
    }
    return &records_.asArray()[static_cast<size_t>(index)];
}

bool JsonStore::update(const std::string& id, JsonValue record)
{
    int index = indexOf(id);
    if (index == -1)
    {
        return false;
    }

    record["id"] = JsonValue(id);
    records_.asArray()[static_cast<size_t>(index)] = std::move(record);
    return true;
}

bool JsonStore::remove(const std::string& id)
{
    int index = indexOf(id);
    if (index == -1)
    {
        return false;
    }

    auto& array = records_.asArray();
    array.erase(array.begin() + index);
    return true;
}

size_t JsonStore::size() const
{
    return records_.asArray().size();
}
