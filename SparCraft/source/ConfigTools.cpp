#include "ConfigTools.h"
#include "torch/TorchTools.h"

using namespace SparCraft;

std::shared_ptr<Map> ConfigTools::GetMapFromVariable(const std::string & mapVariable, const rapidjson::Value & root)
{
    SPARCRAFT_ASSERT(root["Maps"].HasMember(mapVariable.c_str()), "Map variable not found");

    const rapidjson::Value & mapValue = root["Maps"][mapVariable.c_str()];
    SPARCRAFT_ASSERT(mapValue.HasMember("Type"), "Map has no 'Type' option");
    const std::string & mapType = mapValue["Type"].GetString();

    if (mapType == "Arena")
    {
        SPARCRAFT_ASSERT(mapValue.HasMember("Width") && mapValue["Width"].IsInt(), "Map value must have int Width member");
        SPARCRAFT_ASSERT(mapValue.HasMember("Height") && mapValue["Height"].IsInt(), "Map valuemust have int Height member");

        int buildWidth = mapValue["Width"].GetInt() / 32;
        int buildheight = mapValue["Height"].GetInt() / 32;

        return std::shared_ptr<Map>(new Map(buildWidth, buildheight));
    }
    else if (mapType == "TextFile")
    {
        SPARCRAFT_ASSERT(mapValue.HasMember("File") && mapValue["File"].IsString(), "Map value must have String 'File' member");

        std::shared_ptr<Map> map(new Map());
        map->load(mapValue["File"].GetString());

        return map;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown map type: %s", mapType.c_str());
    }

    return std::shared_ptr<Map>();
}

GameState ConfigTools::GetStateFromVariable(const std::string & stateVariable, const rapidjson::Value & root)
{
    SPARCRAFT_ASSERT(root["States"].HasMember(stateVariable.c_str()), "State variable not found");

    const rapidjson::Value & stateValue = root["States"][stateVariable.c_str()];
    SPARCRAFT_ASSERT(stateValue.HasMember("Type"), "State has no 'Type' option");
    const std::string & stateType = stateValue["Type"].GetString();

    GameState state;

    if (stateType == "Symmetric")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("Border") && stateValue["Border"].IsArray(), "Separated State required 'Border' array");
        SPARCRAFT_ASSERT(stateValue.HasMember("Centers") && stateValue["Centers"].IsArray(), "Separated State required 'Centers' array");
        SPARCRAFT_ASSERT(stateValue.HasMember("Units") && stateValue["Units"].IsArray(), "Separated State required 'Units' array");

        // parse the border
        const rapidjson::Value & borderValue = stateValue["Border"];
        SPARCRAFT_ASSERT(borderValue.Size() == 2 && borderValue[0].IsInt() && borderValue[1].IsInt(), "Separated State 'Border' must be array of size 2 [int, int]");
        int border[2] = { borderValue[0].GetInt(), borderValue[1].GetInt() };

        // parse the centers
        const rapidjson::Value & centers = stateValue["Centers"];
        SPARCRAFT_ASSERT(centers.Size() == 2 && centers[0].IsArray() && centers[1].IsArray(), "Separated State 'Centers' must be array of 2 arrays of 2 ints each [[int,int],[int][int]]");
        int center[2][2] = { { centers[0][0].GetInt(), centers[0][1].GetInt() }, { centers[1][0].GetInt(), centers[1][1].GetInt() } };

        // parse the units
        const rapidjson::Value & units = stateValue["Units"];
        for (size_t i(0); i < units.Size(); ++i)
        {
            SPARCRAFT_ASSERT(units[i].IsArray() && units[i].Size() == 2, "Units array member must be array of size 2");
            SPARCRAFT_ASSERT(units[i][0].IsString() && units[i][1].IsInt(), "Unis array member must be [UnitType (String), Number (int)]");

            BWAPI::UnitType type = BWAPI::UnitType::getType(units[i][0].GetString());
            int num = units[i][1].GetInt();

            // add the symmetric unit for each count in the numUnits Vector
            for (int u(0); u < num; ++u)
            {
                Position r((rand() % (2 * border[0])) - border[0], (rand() % (2 * border[1])) - border[1]);
                Position u1(center[0][0] + r.x(), center[0][1] + r.y());
                Position u2(center[1][0] - r.x(), center[1][1] - r.y());

                state.addUnit(SparCraft::Unit(type, Players::Player_One, u1));
                state.addUnit(SparCraft::Unit(type, Players::Player_Two, u2));
            }
        };
    }
    else if (stateType == "StateFile")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "StateFile must have 'File' String member");

        std::ifstream fin(stateValue["File"].GetString());

        SPARCRAFT_ASSERT(fin.good(), "Couldn't open State File: %s", stateValue["File"].GetString());

        std::string unitType;
        size_t player = 0, posX = 0, posY = 0;

        while (fin >> unitType >> player >> posX >> posY)
        {
            Unit unit(BWAPI::UnitType::getType(unitType), player, Position(posX, posY));
            state.addUnit(unit);
        }
    }
    else if (stateType == "TorchCraftFrame")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "TorchCraftFrame must have 'File' String member");

        return TorchTools::GetSparCraftStateFromTorchCraftFrameFile(stateValue["File"].GetString());
    }
    else if (stateType == "TorchCraftStdIn")
    {
        SPARCRAFT_ASSERT(stateValue.HasMember("File") && stateValue["File"].IsString(), "TorchCraftFrame must have 'File' String member");

        std::ifstream fin(stateValue["File"].GetString());

        std::string type, aiPlayerName;
        int playerID = 0, mapWidth = 0, mapHeight = 0;

        fin >> type >> aiPlayerName >> playerID >> mapWidth >> mapHeight;

        GameState state = TorchTools::GetSparCraftStateFromTorchCraftFrameStream(fin);
        state.setMap(std::shared_ptr<Map>(new Map(mapWidth / 4, mapHeight / 4)));

        return state;
    }
    else
    {
        SPARCRAFT_ASSERT(false, "Unknown state type: %s", stateType.c_str());
    }

    if (stateValue.HasMember("Map") && stateValue["Map"].IsString())
    {
        state.setMap(GetMapFromVariable(stateValue["Map"].GetString(), root));
    }

    return state;
}
