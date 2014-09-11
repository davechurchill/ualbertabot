#include "PrerequisiteSet.h"

using namespace BOSS;

ActionCountPair::ActionCountPair()
    : _count(0)
{

}

ActionCountPair::ActionCountPair(const ActionType & action, const UnitCountType count)
    : _action(action)
    , _count(count)
{
}

const ActionType & ActionCountPair::getAction() const
{
    return _action;
}

const UnitCountType & ActionCountPair::getCount() const
{
    return _count;       
}

PrerequisiteSet::PrerequisiteSet()
{

}

const size_t PrerequisiteSet::size() const
{
    return _actionCounts.size();
}

const bool PrerequisiteSet::isEmpty() const
{
    return size() == 0;
}

const bool PrerequisiteSet::contains(const ActionType & action) const
{
    for (size_t i(0); i<_actionCounts.size(); ++i)
    {
        if (getActionType(i) == action)
        {
            return true;
        }
    }

    return false;
}

const ActionType & PrerequisiteSet::getActionType(const UnitCountType index) const
{
    return _actionCounts[index].getAction();
}

const UnitCountType & PrerequisiteSet::getActionTypeCount(const UnitCountType index) const
{
    return _actionCounts[index].getCount();
}
    
void PrerequisiteSet::add(const ActionType & action, const UnitCountType count)
{
    _actionCounts.push_back(ActionCountPair(action, count));
}

void PrerequisiteSet::addUnique(const ActionType & action, const UnitCountType count)
{
    if (!contains(action))
    {
        add(action, count);
    }
}

void PrerequisiteSet::addUnique(const PrerequisiteSet & set)
{
    for (size_t i(0); i<set.size(); ++i)
    {
        addUnique(set.getActionType(i), set.getActionTypeCount(i));
    }
}

void PrerequisiteSet::remove(const ActionType & action)
{
    for (size_t i(0); i<_actionCounts.size(); ++i)
    {
        if (_actionCounts[i].getAction() == action)
        {
            _actionCounts.remove(i);
            return;
        }
    }
}

void PrerequisiteSet::remove(const PrerequisiteSet & set)
{
    if (isEmpty())
    {
        return;
    }

    for (size_t i(0); i < set.size(); ++i)
    {
        remove(set.getActionType(i));
    }
}

const std::string PrerequisiteSet::toString() const
{
    std::stringstream ss;
    
    for (size_t i(0); i<size(); ++i)
    {
        ss << "    Prereq:   " << (int)getActionTypeCount(i) << " " << getActionType(i).getName() << "\n";
    }

    return ss.str();
}