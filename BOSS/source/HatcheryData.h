#pragma once

#include "Common.h"

#include <string.h>
#include <queue>
#include <algorithm>

#include "PrerequisiteSet.h"
#include "Array.hpp"
#include "ActionType.h"

namespace BOSS
{

class Hatchery
{ 
    UnitCountType           _numLarva;
	
public:
	
	Hatchery(const UnitCountType & numLarva);
	Hatchery();

       
    void                    useLarva();
    void                    fastForward(const FrameCountType & currentFrame, const FrameCountType & toFrame);

    const UnitCountType &   numLarva() const;
};

class HatcheryData
{
    Vec<Hatchery, Constants::MAX_HATCHERIES> _hatcheries;

public:

    HatcheryData();

    void                    addHatchery(const UnitCountType & numLarva);
	void                    removeHatchery();
    void                    useLarva();
    void                    fastForward(const FrameCountType & currentFrame, const FrameCountType & toFrame);

    const FrameCountType    nextLarvaFrameAfter(const FrameCountType & currentFrame) const;
    const UnitCountType     numLarva() const;
    const UnitCountType     size() const;
    const Hatchery &        getHatchery(const UnitCountType & index) const;
};

}