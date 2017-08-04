#pragma once

#include "Common.h"
#include <BWAPI/Unit.h>
#include "OpponentView.h"
#include "WorkerManager.h"

namespace UAlbertaBot
{

class AutoObserver
{
    int                         _cameraLastMoved;
    int                         _unitFollowFrames;
    BWAPI::Unit      _observerFollowingUnit;
	const WorkerManager& _workerManager;
	const AKBot::OpponentView& _opponentView;

public:

    AutoObserver(const AKBot::OpponentView& opponentView, const WorkerManager& workerManager);
    void onFrame(int currentFrame);
};

}