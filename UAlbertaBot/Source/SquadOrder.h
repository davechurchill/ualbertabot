#pragma once

#include "Common.h"

namespace UAlbertaBot
{

namespace SquadOrderTypes
{
    enum { None, Idle, Attack, Defend, Regroup, Drop, SquadOrderTypes };
}

class SquadOrder
{
    size_t              m_type       = SquadOrderTypes::None;
    int                 m_radius     = 0;
    BWAPI::Position     m_position;
    std::string         m_status;

public:

	SquadOrder() 
	{
	}

	SquadOrder(int type, BWAPI::Position position, int radius, std::string status = "Default") 
		: m_type(type)
		, m_position(position)
		, m_radius(radius) 
		, m_status(status)
	{
	}

	const std::string & getStatus() const 
	{
		return m_status;
	}

    const BWAPI::Position & getPosition() const
    {
        return m_position;
    }

    const int getRadius() const
    {
        return m_radius;
    }

    const size_t getType() const
    {
        return m_type;
    }
};
}