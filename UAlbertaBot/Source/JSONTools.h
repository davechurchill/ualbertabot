#pragma once

#include "Common.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace UAlbertaBot
{
namespace JSONTools
{
    template <class T>
    void ReadInt(const char * key, const rapidjson::Value & value, T & dest)
    {
        if (value.HasMember(key))
        {
            UAB_ASSERT(value[key].IsInt(), "%s should be an int", key);
            dest = (T)value[key].GetInt();
        }
    }

    void ReadBool(const char * key, const rapidjson::Value & value, bool & dest);
    void ReadString(const char * key, const rapidjson::Value & value, std::string & dest);
}
}
