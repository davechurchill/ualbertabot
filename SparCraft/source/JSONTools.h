#pragma once

#include "Common.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace SparCraft
{
namespace JSONTools
{
    template <class T>
    void ReadInt(const char * key, const rapidjson::Value & value, T & dest, bool assertExists = false)
    {
        if (assertExists)
        {
            SPARCRAFT_ASSERT(value.HasMember(key), "Key not found: %s", key);
        }

        if (value.HasMember(key))
        {
            if (value[key].IsInt())
            {
                dest = (T)value[key].GetInt();
            }
            else
            {
                SPARCRAFT_ASSERT(false, "%s should be an int/string with int contents", key);
            }
        }
    }

    void ReadIntBool(const char * key, const rapidjson::Value & value, bool & dest, bool assertExists = false);
    void ReadBool(const char * key, const rapidjson::Value & value, bool & dest, bool assertExists = false);
    void ReadDouble(const char * key, const rapidjson::Value & value, double & dest, bool assertExists = false);
    void ReadString(const char * key, const rapidjson::Value & value, std::string & dest, bool assertExists = false);
}
}