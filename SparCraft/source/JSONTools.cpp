#include "JSONTools.h"

using namespace SparCraft;

void JSONTools::ReadIntBool(const char * key, const rapidjson::Value & value, bool & dest, bool assertExists)
{
    if (assertExists)
    {
        SPARCRAFT_ASSERT(value.HasMember(key), "Key not found: %s", key);
    }

    if (value.HasMember(key))
    {
        SPARCRAFT_ASSERT(value[key].IsInt(), "%s should be an int", key);
        dest = value[key].GetInt() != 0;
    }
}

void JSONTools::ReadBool(const char * key, const rapidjson::Value & value, bool & dest, bool assertExists)
{
    if (assertExists)
    {
        SPARCRAFT_ASSERT(value.HasMember(key), "Key not found: %s", key);
    }

    if (value.HasMember(key))
    {
        SPARCRAFT_ASSERT(value[key].IsBool(), "%s should be a bool", key);
        dest = value[key].GetBool();
    }
}

void JSONTools::ReadDouble(const char * key, const rapidjson::Value & value, double & dest, bool assertExists)
{
    if (assertExists)
    {
        SPARCRAFT_ASSERT(value.HasMember(key), "Key not found: %s", key);
    }

    if (value.HasMember(key))
    {
        SPARCRAFT_ASSERT(value[key].IsDouble(), "%s should be a double", key);
        dest = value[key].GetDouble();
    }
}

void JSONTools::ReadString(const char * key, const rapidjson::Value & value, std::string & dest, bool assertExists)
{
    if (assertExists)
    {
        SPARCRAFT_ASSERT(value.HasMember(key), "Key not found: %s", key);
    }

    if (value.HasMember(key))
    {
        SPARCRAFT_ASSERT(value[key].IsString(), "%s should be a string", key);
        dest = value[key].GetString();
    }
}