#include "ActionSet.h"

using namespace BOSS;

ActionSet::ActionSet()
{

}

const size_t ActionSet::size() const
{
    return _actionTypes.size();
}

const bool ActionSet::isEmpty() const
{
    return size() == 0;
}

const ActionType & ActionSet::operator [] (const size_t & index) const
{
    return _actionTypes[index];
}

ActionType & ActionSet::operator [] (const size_t & index)
{
    return _actionTypes[index];
}

const bool ActionSet::contains(const ActionType & action) const
{
    return _actionTypes.contains(action);
}

void ActionSet::add(const ActionType & action)
{
    _actionTypes.push_back(action);
}

void ActionSet::remove(const ActionType & action)
{
    for (size_t i(0); i<_actionTypes.size(); ++i)
    {
        if (_actionTypes[i] == action)
        {
            _actionTypes.removeByShift(i);
            return;
        }
    }
}

void ActionSet::clear()
{
    _actionTypes.clear();
}