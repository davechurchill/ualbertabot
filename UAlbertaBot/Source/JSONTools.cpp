#include "JSONTools.h"
#include "UABAssert.h"
#include "Common.h"
#include <sstream>

namespace UAlbertaBot
{
namespace JSONTools
{
    void ReadBool(const char * key, const rapidjson::Value & value, bool & dest)
    {
        if (value.HasMember(key))
        {
            UAB_ASSERT(value[key].IsBool(), "%s should be a bool", key);
            dest = value[key].GetBool();
        }
    }

    void ReadString(const char * key, const rapidjson::Value & value, std::string & dest)
    {
        if (value.HasMember(key))
        {
            UAB_ASSERT(value[key].IsString(), "%s should be a string", key);
            dest = value[key].GetString();
        }
    }
}

std::string JSONTools::GetShortName(const std::string & name)
{
    std::cout << name;
    std::stringstream ss;
    bool firstUnderscore = true;

    if (name.find("Protoss") == 0 || name.find("Terran") == 0 || name.find("Zerg") == 0)
    {
        firstUnderscore = false;
    }
    
    for (auto & c : name)
    {
        if (c == '_') { firstUnderscore = true; }
        if (c != '_' && firstUnderscore) { ss << c; }
    }

    std::cout << " " << ss.str() << "\n";
    return ss.str();
}

std::string ToJSON(const BWAPI::UnitType & type)
{
    bool supplyProvider = (type == BWAPI::UnitTypes::Terran_Supply_Depot || type == BWAPI::UnitTypes::Zerg_Overlord || type == BWAPI::UnitTypes::Protoss_Pylon);
    std::stringstream ss;

    std::string prefix = "            ";
    std::string suffix = "\n";
    std::string name = JSONTools::GetShortName(type.getName());
    int produced = 1;
    if (type == BWAPI::UnitTypes::Zerg_Zergling || type == BWAPI::UnitTypes::Zerg_Scourge) { produced = 2; }

    int startingEnergy = 0;
    if (type.maxEnergy() > 0) { startingEnergy = 50; }
    if (type == BWAPI::UnitTypes::Protoss_Shield_Battery) { startingEnergy = 100; }

    ss << "{ " << suffix;
    ss << prefix << "\"name\" : \"" << name << "\", " << suffix;
    ss << prefix << "\"race\" : \"" << type.getRace() << "\", " << suffix;
    ss << prefix << "\"mineralCost\" : " << type.mineralPrice() << ", " << suffix,
    ss << prefix << "\"gasCost\" : " << type.gasPrice() << ", " << suffix;
    ss << prefix << "\"supplyCost\" : " << type.supplyRequired() << ", " << suffix;
    ss << prefix << "\"energyCost\" : " << 0 << ", " << suffix; 
    ss << prefix << "\"supplyProvided\" : " << type.supplyProvided() << ", " << suffix;
    ss << prefix << "\"buildTime\" : " << type.buildTime() << ", " << suffix;
    ss << prefix << "\"numProduced\" : " << produced << ", " << suffix;
    ss << prefix << "\"startingEnergy\" : " << startingEnergy << ", " << suffix;
    ss << prefix << "\"maxEnergy\" : " << type.maxEnergy() << ", " << suffix;
    ss << prefix << "\"isUnit\" : " << "true, " << suffix;
    ss << prefix << "\"isUpgrade\" : " << "false, " << suffix;
    ss << prefix << "\"isAbility\" : " << "false, " << suffix;
    ss << prefix << "\"isBuilding\" : " << (type.isBuilding() ? "true" : "false") << ", " << suffix;
    ss << prefix << "\"isWorker\" : " << (type.isWorker() ? "true" : "false")   << ", " << suffix;
    ss << prefix << "\"isRefinery\" : " << (type.isRefinery() ? "true" : "false")   << ", " << suffix;
    ss << prefix << "\"isSupplyProvider\" : " << (supplyProvider ? "true" : "false")   << ", " << suffix;
    ss << prefix << "\"isResourceDepot\" : " << (type.isResourceDepot() ? "true" : "false")   << ", " << suffix;
    ss << prefix << "\"isAddon\" : " << (type.isAddon() ? "true" : "false")   << ", " << suffix;

    std::vector<std::string> whatBuilds;
    whatBuilds.push_back(type.whatBuilds().first.getName());
    std::vector<std::string> equivalent;
    
    // handle cases where the thing that builds it can be morphed
    if (name == "Hatchery")
    {
        equivalent.push_back("Lair");
        equivalent.push_back("Hive");
    }
    else if (name == "Lair")
    {
        equivalent.push_back("Hive");
    }
    else if (name == "Spire")
    {
        equivalent.push_back("GreaterSpire");
    }

    std::vector<std::string> required;
    std::string whatBuildsAddon = "";
    std::string requiredAddon = "";

    if (type.getRace() == BWAPI::Races::Protoss && type.isBuilding() && !type.isRefinery() && type.supplyProvided() == 0)
    {
        required.push_back("Pylon");
    }

    for (auto & req : type.requiredUnits())
    {
        // skip the builder since it's already in whatBuilds
        if (req.first == type.whatBuilds().first) { continue; }
        if (req.first.isAddon())
        {
            if (req.first.whatBuilds().first == type.whatBuilds().first)
            {
                whatBuildsAddon = JSONTools::GetShortName(req.first.getName());
            }
            else
            {
                requiredAddon = JSONTools::GetShortName(req.first.getName());
            }

            continue;
        }

        required.push_back(JSONTools::GetShortName(req.first.getName()));
    }

    if (!requiredAddon.empty())
    {
        required.push_back(requiredAddon);
    }

    if (type.requiredTech() != BWAPI::TechTypes::None)
    {
        required.push_back(JSONTools::GetShortName(type.requiredTech().getName()));
    }

    ss << prefix << "\"equivalent\" : [";
    for (size_t i(0); i < equivalent.size(); ++i)
    { 
        ss << "\"" << equivalent[i] << "\"";
        if (i < equivalent.size() - 1) { ss << ", "; }
    }
    ss << "], " << suffix;
    
    ss << prefix << "\"whatBuilds\" : " << "[\"" << JSONTools::GetShortName(type.whatBuilds().first.getName()) << "\", " << type.whatBuilds().second;

    if (type.getRace() == BWAPI::Races::Zerg || type == BWAPI::UnitTypes::Protoss_Archon || type == BWAPI::UnitTypes::Protoss_Dark_Archon)
    {
        ss << ", \"Morphed\"";
    }
    else if (type.getRace() == BWAPI::Races::Terran)
    {
        ss << ", \"Busy\"";
    }
    else
    {
        if (type.isBuilding()) { ss << ", \"None\""; }
        else { ss << ", \"Busy\""; }
    }

    if (whatBuildsAddon.size() > 0)
    {
        ss << ", \"" << whatBuildsAddon << "\"";
    }   ss << "], " << suffix;
    
    ss << prefix << "\"required\" : [";
    for (size_t i(0); i < required.size(); ++i)
    { 
        ss << "\"" << required[i] << "\"";
        if (i < required.size() - 1) { ss << ", "; }
    }   ss << "]" << suffix;

    ss << " }";

    return ss.str();
}

void JSONTools::OutputTypeDataJSON(const std::string & filename)
{
    std::ofstream out(filename);
    out << "{\n";
    std::string q = "\"";
    std::string qcs = "\", ";

    
    out << "    \"Game\" : \"StarCraft\",\n";
    out << "    \"WorkersPerRefinery\" : " << 3 << ",\n";
    out << "    \"MineralsPerWorkerPerFrame\" : " << 0.045 << ",\n";
    out << "    \"GasPerWorkerPerFrame\" : " << 0.070 << ",\n";
    out << "    \"EnergyRegenPerFrame\" : " << 0.031 << ",\n";
    out << "    \"HealthRegenPerFrame\" : " << 0.0155 << ",\n";
    out << "    \"ShieldRegenPerFrame\" : " << 0.027125 << ",\n";
    out << "    \"Types\" : \n    [\n";

    auto & unitTypes = BWAPI::UnitTypes::allUnitTypes();
    std::vector<BWAPI::UnitType> goodTypes;
    size_t i = 0;
    for (auto & data : unitTypes)
    {
        if (data.getName().find("Protoss") == std::string::npos && data.getName().find("Zerg") == std::string::npos && data.getName().find("Terran") == std::string::npos) { continue; }
        if (data.getName().find("Type") != std::string::npos) { continue; }
        if (data == BWAPI::UnitTypes::Zerg_Egg || data == BWAPI::UnitTypes::Zerg_Lurker_Egg ||
            data == BWAPI::UnitTypes::Zerg_Cocoon || data == BWAPI::UnitTypes::Zerg_Infested_Terran ||
            data == BWAPI::UnitTypes::Zerg_Infested_Command_Center || data == BWAPI::UnitTypes::Zerg_Broodling ||
            data == BWAPI::UnitTypes::Protoss_Interceptor || data == BWAPI::UnitTypes::Protoss_Scarab ||
            data == BWAPI::UnitTypes::Terran_Civilian || data == BWAPI::UnitTypes::Terran_Nuclear_Missile || 
            data == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine || data.isSpecialBuilding())
        {
            continue;
        }
        
        goodTypes.push_back(data);
        
    }

    for (size_t i(0); i < goodTypes.size(); ++i)
    {
        out << "        " << ToJSON(goodTypes[i]);
        if (i < goodTypes.size() - 1) { out << ",\n"; }
    }

    out << "    ]\n";
    out << "}";
    out.close();
}

}

