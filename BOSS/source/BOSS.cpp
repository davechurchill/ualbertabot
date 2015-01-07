#include "BOSS.h"

namespace BOSS
{
    void init()
    {
        // Initialize data

        // initialize BWAPI from BWAPIDATA if we're not in windows
        #ifndef _MSC_VER
            BWAPI::BWAPI_init();
        #endif
        ActionTypeData::Init();
        ActionTypes::init();
    }

    void printData()
    {
        for (RaceID r(0); r < Races::NUM_RACES; ++r)
        {
            const std::vector<ActionType> & allActions = ActionTypes::GetAllActionTypes(r);
            for (size_t i(0); i<allActions.size(); ++i)
            {
                const ActionType & a = allActions[i];
                std::cout << "\n" << (int)a.getRace() << " " << (int)a.ID() << " " << a.getName() << "\n\n";
                std::cout << "    Built@:   " << a.whatBuildsActionType().getName() << "\n";
                if (a.requiresAddon())
                {
                    std::cout << "    Addon :   " << a.requiredAddonType().getName() << "\n";
                }
                
                std::cout << a.getPrerequisites().toString();
            }
        }
    }
}