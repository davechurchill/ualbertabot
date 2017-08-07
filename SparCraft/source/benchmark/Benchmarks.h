#pragma once

#include "../SparCraft.h"
#include "../rapidjson/rapidjson.h"
//#include "PlayerBenchmark.h"

namespace SparCraft
{

namespace Benchmarks
{
    void DoBenchmarks(const std::string & filename);

    void DoTournamentBenchmark(const rapidjson::Value & value, const rapidjson::Value & rootValue);
    //void DoPlayerBenchmark(const PlayerBenchmark & benchmark);
}

}
