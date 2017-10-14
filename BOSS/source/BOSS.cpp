#include "BOSS.h"

namespace BOSS
{
    bool isInit = false;

    void init()
    {
        if (!isInit)
        {
            std::cout << "Initializing BOSS...\n";

            ActionTypeData::Init();
            ActionTypes::init();
            isInit = true;
        }
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