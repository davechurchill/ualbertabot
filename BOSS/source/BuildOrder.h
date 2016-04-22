#pragma once

#include "Common.h"
#include "ActionType.h"
#include "GameState.h"

namespace BOSS
{

class BuildOrder
{
    std::vector<ActionType>     _buildOrder;
    std::vector<size_t>         _typeCount;

public:

    BuildOrder();

    void                    add(const ActionType & type);
    void                    add(const ActionType & type, const int & amount);
    void                    add(const BuildOrder & other);
    void                    clear();
    void                    pop_back();
    void                    sortByPrerequisites();

    bool                    doActions(GameState & state, const size_t buildOrderStartIndex = 0) const;
    bool                    doActions(GameState & state, const size_t buildOrderStartIndex, const size_t buildOrderEndIndex) const;

    const ActionType &      operator [] (const size_t & i) const;
    ActionType &            operator [] (const size_t & i);

    const size_t            size() const;
    const size_t            getTypeCount(const ActionType & type) const;
    const bool              isLegalFromState(const GameState & state, const size_t buildOrderStartIndex = 0) const;
    std::string             whyIsNotLegalFromState(const GameState & state, const size_t buildOrderStartIndex = 0) const;
    const bool              empty() const;
    const FrameCountType    getCompletionTime(const GameState & state, const size_t buildOrderStartIndex = 0) const;

    std::string             getJSONString() const;
    std::string             getNumberedString() const;
    std::string             getIDString() const;
    std::string             getNameString(const size_t charactersPerName = 0) const;
};

}