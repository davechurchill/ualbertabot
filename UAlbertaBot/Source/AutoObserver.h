#pragma once

#include "Common.h"

namespace UAlbertaBot
{

class AutoObserver
{
    int         m_cameraLastMoved = 0;
    int         m_unitFollowFrames = 0;
    BWAPI::Unit m_observerFollowingUnit = nullptr;

public:

    AutoObserver();
    void onFrame();
};

}