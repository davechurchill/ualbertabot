#pragma once

#include "Common.h"

namespace UAlbertaBot
{
    
class ArenaPlayer
{

protected:

    std::string _name;

public:

	ArenaPlayer();
    
    virtual const std::string & getName() const;

    virtual void onStart() = 0;
    virtual void onFrame() = 0;
    virtual void onBattleBegin() = 0;
    virtual void onBattleEnd() = 0;
};
}