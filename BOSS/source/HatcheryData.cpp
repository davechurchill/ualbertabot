#include "HatcheryData.h"

using namespace BOSS;

Hatchery::Hatchery(const UnitCountType & numLarva) 
	: _numLarva(numLarva)
{
}

Hatchery::Hatchery() 
	: _numLarva(3)
{
}

void Hatchery::fastForward(const FrameCountType & currentFrame, const FrameCountType & toFrame)
{
    if (_numLarva == 3)
    {
        return;
    }

    UnitCountType  larvaToAdd   = (toFrame / Constants::ZERG_LARVA_TIMER) - (currentFrame / Constants::ZERG_LARVA_TIMER);

    larvaToAdd                  = std::min(larvaToAdd, (UnitCountType)(3 - _numLarva));

    _numLarva += larvaToAdd;
}

void Hatchery::useLarva()
{
    BOSS_ASSERT(_numLarva > 0, "We should have larva to use");

    _numLarva--;
}

const UnitCountType & Hatchery::numLarva() const
{
    return _numLarva;
}

HatcheryData::HatcheryData()
{

}

void HatcheryData::addHatchery(const UnitCountType & numLarva)
{
    _hatcheries.push_back(Hatchery(numLarva));
}

void HatcheryData::removeHatchery()
{
	_hatcheries.pop_back();
}

void HatcheryData::fastForward(const FrameCountType & currentFrame, const FrameCountType & toFrame)
{
    for (size_t i(0); i < _hatcheries.size(); ++i)
    {
        _hatcheries[i].fastForward(currentFrame, toFrame);
    }
}

void HatcheryData::useLarva()
{
    int maxLarvaIndex = -1;
    for (size_t i(0); i < _hatcheries.size(); ++i)
    {
        if (_hatcheries[i].numLarva() > 0)
        {
            if (maxLarvaIndex == -1 || (_hatcheries[i].numLarva() > _hatcheries[maxLarvaIndex].numLarva()))
            {
                maxLarvaIndex = i;
            }
        }
    }

    if (maxLarvaIndex != -1)
    {
        _hatcheries[maxLarvaIndex].useLarva();
    }
    else
    {
        BOSS_ASSERT(false, "Should have found a larva to use");
    }
}

const FrameCountType HatcheryData::nextLarvaFrameAfter(const FrameCountType & currentFrame) const
{
    if (currentFrame % Constants::ZERG_LARVA_TIMER == 0)
    {
        return currentFrame + Constants::ZERG_LARVA_TIMER;
    }
    else
    {
        return Constants::ZERG_LARVA_TIMER * ((currentFrame / Constants::ZERG_LARVA_TIMER) + 1);
    }
}

const UnitCountType HatcheryData::numLarva() const
{
    UnitCountType sumLarva = 0;

    for (size_t i(0); i < _hatcheries.size(); ++i)
    {
        sumLarva += _hatcheries[i].numLarva();
    }

    return sumLarva;
}

const UnitCountType HatcheryData::size() const
{
    return _hatcheries.size();
}

const Hatchery & HatcheryData::getHatchery(const UnitCountType & index) const
{
    return _hatcheries[index];
}