#include "BuildOrder.h"

using namespace BOSS;

BuildOrder::BuildOrder()
{

}

void BuildOrder::add(const ActionType & type)
{
    BOSS_ASSERT((_buildOrder.size() == 0) || (type.getRace() == _buildOrder.back().getRace()), "Cannot have a build order with multiple races");

    // if this is the first item, set up the count vector
    if (_buildOrder.size() == 0)
    {
        _typeCount = std::vector<size_t>(ActionTypes::GetAllActionTypes(type.getRace()).size(), 0);
    }

    _buildOrder.push_back(type);
    _typeCount[type.ID()]++;
}

void BuildOrder::add(const ActionType & type, const int & amount)
{
    for (int i(0); i < amount; ++i)
    {
        add(type);
    }
}

void BuildOrder::add(const BuildOrder & other)
{
    for (size_t i(0); i < other.size(); ++i)
    {
        add(other[i]);
    }
}

void BuildOrder::clear()
{
    _buildOrder.clear();
    _typeCount.clear();
}

const bool BuildOrder::empty() const
{
    return size() == 0;
}

void BuildOrder::sortByPrerequisites()
{
    for (size_t i(0); i < _buildOrder.size()-1; ++i)
    {
        for (size_t j(i+1); j < _buildOrder.size(); ++j)
        {
            const PrerequisiteSet & recursivePre = _buildOrder[i].getRecursivePrerequisites();

            if (recursivePre.contains(_buildOrder[j]))
            {
                std::swap(_buildOrder[i], _buildOrder[j]);
            }
        }
    }
}

const size_t BuildOrder::getTypeCount(const ActionType & type) const
{
    if (empty())
    {
        return 0;
    }

    BOSS_ASSERT(type.getRace() == _buildOrder[0].getRace(), "Trying to get type count of a different race type");

    return _typeCount[type.ID()];
}

void BuildOrder::pop_back()
{
    _buildOrder.pop_back();
}

const ActionType & BuildOrder::operator [] (const size_t & i) const
{
    return _buildOrder[i];
}

ActionType & BuildOrder::operator [] (const size_t & i) 
{
    return _buildOrder[i];
}

const size_t BuildOrder::size() const
{
    return _buildOrder.size();
}

const bool BuildOrder::isLegalFromState(const GameState & state, size_t buildOrderStartIndex) const
{
    GameState currentState(state);

    for (size_t i(buildOrderStartIndex); i < _buildOrder.size(); ++i)
    {
        if (!currentState.isLegal(_buildOrder[i]))
        {
            return false;
        }
        else
        {
            currentState.doAction(_buildOrder[i]);
        }
    }

    return true;
}

std::string BuildOrder::getJSONString() const
{
    std::stringstream ss;

    ss << "\"Build Order\" : [";

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        ss << "\"" << _buildOrder[i].getName() << "\"" << (i < _buildOrder.size() - 1 ? ", " : "");
    }

    ss << "]";

    return ss.str();
}

bool BuildOrder::doActions(GameState & state, const size_t buildOrderStartIndex, const size_t buildOrderEndIndex) const
{
    BOSS_ASSERT(buildOrderEndIndex < _buildOrder.size(), "Can't have this end index");

    for (size_t i(buildOrderStartIndex); i < buildOrderEndIndex; ++i)
    {
        if(!state.isLegal(_buildOrder[i]))
        {
            return false;
        }

        state.doAction(_buildOrder[i]);
    }

    return true;
}

// returns whether or not all the actions were legal
bool BuildOrder::doActions(GameState & state, size_t buildOrderStartIndex) const
{
    for (size_t i(buildOrderStartIndex); i < _buildOrder.size(); ++i)
    {
        if(!state.isLegal(_buildOrder[i]))
        {
            return false;
        }

        state.doAction(_buildOrder[i]);
    }

    return true;
}

const FrameCountType BuildOrder::getCompletionTime(const GameState & state, size_t buildOrderStartIndex) const
{
    if (_buildOrder.size() == 0)
    {
        return state.getLastActionFinishTime();
    }
    
    BOSS_ASSERT(buildOrderStartIndex < _buildOrder.size(), "We can't start at an index that's past the end");

    GameState currentState(state);
    bool isLegal = doActions(currentState, buildOrderStartIndex);

    BOSS_ASSERT(isLegal, "Build order was not legal");

    return currentState.getLastActionFinishTime();
}

std::string BuildOrder::getNumberedString() const
{
    std::stringstream ss;

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        std::stringstream num;
        num << i;
        while (num.str().length() < 5)
        {
            num << " ";
        }

        ss << num.str() << _buildOrder[i].getName() << std::endl;
    }

    return ss.str();
}

std::string BuildOrder::getIDString() const
{
    std::stringstream ss;

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        ss << (int)_buildOrder[i].ID() << " ";
    }

    return ss.str();
}

std::string BuildOrder::getNameString(const size_t charactersPerName) const
{
    std::stringstream ss;

    for (size_t i(0); i < _buildOrder.size(); ++i)
    {
        std::string name = charactersPerName == 0 ? _buildOrder[i].getShortName() : _buildOrder[i].getShortName().substr(0, charactersPerName);

        ss << name << " ";
    }

    return ss.str();
}