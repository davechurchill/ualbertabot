#pragma once

#include "Common.h"

namespace UAlbertaBot
{
class SquadOrder
{
public:

	enum { None, Attack, Defend, Regroup, SquadOrderTypes };

	int					type;
	BWAPI::Position		position;
	int					radius;
	std::string			status;

	SquadOrder() 
		: type(None)
        , radius(0)
	{
	}

	SquadOrder(int type, BWAPI::Position position, int radius, std::string stat = "Default") 
		: type(type)
		, position(position)
		, radius(radius) 
		, status(stat)
	{
	}

	std::string getStatus() const 
	{
		return status;
	}
};
}