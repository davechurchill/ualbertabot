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
	shared_ptr<WorkerManager> _workerManager;
	shared_ptr<AKBot::OpponentView> _opponentView;

public:

    AutoObserver(shared_ptr<AKBot::OpponentView> opponentView, shared_ptr<WorkerManager> workerManager);
    void onFrame(int currentFrame);
};

}