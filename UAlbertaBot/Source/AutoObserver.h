#pragma once

#include "Common.h"

namespace UAlbertaBot
{

class AutoObserver
{
    int                         _cameraLastMoved;
    int                         _unitFollowFrames;
    BWAPI::Unit      _observerFollowingUnit;

public:

    AutoObserver();
    void onFrame();
};

}