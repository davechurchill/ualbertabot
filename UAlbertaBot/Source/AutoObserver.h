#pragma once

#include "Common.h"

namespace UAlbertaBot
{

class AutoObserver
{
    int                         _cameraLastMoved;
    int                         _unitFollowFrames;
    BWAPI::UnitInterface *      _observerFollowingUnit;

public:

    AutoObserver();
    void onFrame();
};

}