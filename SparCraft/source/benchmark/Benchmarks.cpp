#include "Benchmarks.h"
#include "Tournament.h"

using namespace SparCraft;

void Benchmarks::DoBenchmarks(const std::string & filename)
{
    rapidjson::Document document;
    bool parsingFailed = document.Parse(FileUtils::ReadFile(filename).c_str()).HasParseError();

    SPARCRAFT_ASSERT(!parsingFailed, "Couldn't parse benchmarks file");
    SPARCRAFT_ASSERT(document.HasMember("Benchmarks"), "JSON has no Benchmarks member");
    SPARCRAFT_ASSERT(document["Benchmarks"].IsArray(), "JSON Benchmarks member is not an array");

    const rapidjson::Value & benchmarks = document["Benchmarks"];
    for (size_t b(0); b < benchmarks.Size(); ++b)
    {
        bool run = benchmarks[b].HasMember("Run") && benchmarks[b]["Run"].IsBool() && benchmarks[b]["Run"].GetBool();
       
        SPARCRAFT_ASSERT(benchmarks[b].HasMember("Name") && benchmarks[b]["Name"].IsString(), "Benchmark must have name string member");
        const std::string & name = benchmarks[b]["Name"].GetString();
        
        // must have a true 'run' bool to run the benchmark
        if (!run)
        {
            continue;
        }

        SPARCRAFT_ASSERT(benchmarks[b].HasMember("Type") && benchmarks[b]["Type"].IsString(), "Benchmark must have type string member");

        const std::string & benchmarkType = benchmarks[b]["Type"].GetString();

        if (benchmarkType == "PlayerBenchmark")
        {
            //PlayerBenchmark bm(benchmarks[b]);
            //DoPlayerBenchmark(bm);
        }
        else if (benchmarkType == "Tournament")
        {
            DoTournamentBenchmark(benchmarks[b], document);
        }
        else
        {
            SPARCRAFT_ASSERT(false, "Unknown Benchmark type: %s", benchmarkType.c_str());
        }
    }
}

void Benchmarks::DoTournamentBenchmark(const rapidjson::Value & value, const rapidjson::Value & rootValue)
{
    Tournament tournament;
    tournament.run(value, rootValue);
}

//void Benchmarks::DoPlayerBenchmark(const PlayerBenchmark & benchmark)
//{
//    PlayerBenchmark b(benchmark);
//    b.run();
//}
