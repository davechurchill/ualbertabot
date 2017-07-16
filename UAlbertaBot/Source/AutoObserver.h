#pragma once

#include "Common.h"
#include <BWAPI/Unit.h>
#include "OpponentView.h"

namespace UAlbertaBot
{

class AutoObserver
{
    int                         _cameraLastMoved;
    int                         _unitFollowFrames;
    BWAPI::Unit      _observerFollowingUnit;
	const AKBot::OpponentView& _opponentView;

public:

    AutoObserver(const AKBot::OpponentView& opponentView);
    void onFrame(int currentFrame);
};

}