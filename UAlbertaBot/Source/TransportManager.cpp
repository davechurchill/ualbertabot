#include "TransportManager.h"

using namespace UAlbertaBot;

TransportManager::TransportManager() :
	_transportShip(NULL)
	, _currentRegionVertexIndex(-1)
	, _minCorner(-1,-1)
	, _maxCorner(-1,-1)
	, _to(-1,-1)
	, _from(-1,-1)
{
}

void TransportManager::executeMicro(const BWAPI::Unitset & targets) 
{
	const BWAPI::Unitset & transportUnits = getUnits();

	if (transportUnits.empty())
	{
		return;
	}	
}

void TransportManager::calculateMapEdgeVertices()
{
	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

	if (!enemyBaseLocation)
	{
		return;
	}

	const BWAPI::Position basePosition = BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
	const std::vector<BWAPI::TilePosition> & closestTobase = MapTools::Instance().getClosestTilesTo(basePosition);

	std::set<BWAPI::Position> unsortedVertices;

	int minX = std::numeric_limits<int>::max(); int minY = minX;
	int maxX = std::numeric_limits<int>::min(); int maxY = maxX;

	//compute mins and maxs
	for(auto &tile : closestTobase)
	{
		if (tile.x > maxX) maxX = tile.x;
		else if (tile.x < minX) minX = tile.x;

		if (tile.y > maxY) maxY = tile.y;
		else if (tile.y < minY) minY = tile.y;
	}

	_minCorner = BWAPI::Position(minX, minY) * 32 + BWAPI::Position(16, 16);
	_maxCorner = BWAPI::Position(maxX, maxY) * 32 + BWAPI::Position(16, 16);

	//add all(some) edge tiles! 
	for (int _x = minX; _x <= maxX; _x += 5)
	{
		unsortedVertices.insert(BWAPI::Position(_x, minY) * 32 + BWAPI::Position(16, 16));
		unsortedVertices.insert(BWAPI::Position(_x, maxY) * 32 + BWAPI::Position(16, 16));
	}

	for (int _y = minY; _y <= maxY; _y += 5)
	{
		unsortedVertices.insert(BWAPI::Position(minX, _y) * 32 + BWAPI::Position(16, 16));
		unsortedVertices.insert(BWAPI::Position(maxX, _y) * 32 + BWAPI::Position(16, 16));
	}

	std::vector<BWAPI::Position> sortedVertices;
	BWAPI::Position current = *unsortedVertices.begin();

	_mapEdgeVertices.push_back(current);
	unsortedVertices.erase(current);

	// while we still have unsorted vertices left, find the closest one remaining to current
	while (!unsortedVertices.empty())
	{
		double bestDist = 1000000;
		BWAPI::Position bestPos;

		for (const BWAPI::Position & pos : unsortedVertices)
		{
			double dist = pos.getDistance(current);

			if (dist < bestDist)
			{
				bestDist = dist;
				bestPos = pos;
			}
		}

		current = bestPos;
		sortedVertices.push_back(bestPos);
		unsortedVertices.erase(bestPos);
	}
    
	_mapEdgeVertices = sortedVertices;
}

void TransportManager::drawTransportInformation(int x = 0, int y = 0)
{
	if (!Config::Debug::DrawUnitTargetInfo)
	{
		return;
	}

	if (x && y)
	{
		//BWAPI::Broodwar->drawTextScreen(x, y, "ScoutInfo: %s", _scoutStatus.c_str());
		//BWAPI::Broodwar->drawTextScreen(x, y + 10, "GasSteal: %s", _gasStealStatus.c_str());
	}
	for (size_t i(0); i < _mapEdgeVertices.size(); ++i)
	{
		BWAPI::Broodwar->drawCircleMap(_mapEdgeVertices[i], 4, BWAPI::Colors::Green, false);
		BWAPI::Broodwar->drawTextMap(_mapEdgeVertices[i], "%d", i);
	}
}

void TransportManager::update()
{
    if (!_transportShip && getUnits().size() > 0)
    {
        _transportShip = *getUnits().begin();
    }

	// calculate enemy region vertices if we haven't yet
	if (_mapEdgeVertices.empty())
	{
		calculateMapEdgeVertices();
	}

	moveTroops();
	moveTransport();
	
	drawTransportInformation();
}

void TransportManager::moveTransport()
{
	if (!_transportShip || !_transportShip->exists() || !(_transportShip->getHitPoints() > 0))
	{
		return;
	}

	// If I didn't finish unloading the troops, wait
	BWAPI::UnitCommand currentCommand(_transportShip->getLastCommand());
	if ((currentCommand.getType() == BWAPI::UnitCommandTypes::Unload_All 
	 || currentCommand.getType() == BWAPI::UnitCommandTypes::Unload_All_Position)
	 && _transportShip->getLoadedUnits().size() > 0)
	{
		return;
	}
	
	if (_to.isValid() && _from.isValid())
	{
		followPerimeter(_to, _from);
	}
	else
	{
		followPerimeter();
	}
}

void TransportManager::moveTroops()
{
	if (!_transportShip || !_transportShip->exists() || !(_transportShip->getHitPoints() > 0))
	{
		return;
	}
	//unload zealots if close enough or dying
	int transportHP = _transportShip->getHitPoints() + _transportShip->getShields();
	
	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

	if (enemyBaseLocation && (_transportShip->getDistance(enemyBaseLocation->getPosition()) < 300 || transportHP < 100)
		&& _transportShip->canUnloadAtPosition(_transportShip->getPosition()))
	{
		//unload troops 
		//and return? 

		// get the unit's current command
		BWAPI::UnitCommand currentCommand(_transportShip->getLastCommand());

		// if we've already told this unit to unload, wait
		if (currentCommand.getType() == BWAPI::UnitCommandTypes::Unload_All || currentCommand.getType() == BWAPI::UnitCommandTypes::Unload_All_Position)
		{
			return;
		}

		//else unload
		_transportShip->unloadAll(_transportShip->getPosition());
	}
	
}

void TransportManager::followPerimeter(int clockwise)
{
	BWAPI::Position goTo = getFleePosition(clockwise);

	if (Config::Debug::DrawUnitTargetInfo)
	{
		BWAPI::Broodwar->drawCircleMap(goTo, 5, BWAPI::Colors::Red, true);
	}

	Micro::SmartMove(_transportShip, goTo);
}

void TransportManager::followPerimeter(BWAPI::Position to, BWAPI::Position from)
{
	static int following = 0;
	if (following)
	{
		followPerimeter(following);
		return;
	}

	//assume we're near FROM! 
	if (_transportShip->getDistance(from) < 50 && _waypoints.empty())
	{
		//compute waypoints
		std::pair<int, int> wpIDX = findSafePath(to, from);
		bool valid = (wpIDX.first > -1 && wpIDX.second > -1);
		UAB_ASSERT_WARNING(valid, "waypoints not valid! (transport mgr)");
		_waypoints.push_back(_mapEdgeVertices[wpIDX.first]);
		_waypoints.push_back(_mapEdgeVertices[wpIDX.second]);

		BWAPI::Broodwar->printf("WAYPOINTS: [%d] - [%d]", wpIDX.first, wpIDX.second);

		Micro::SmartMove(_transportShip, _waypoints[0]);
	}
	else if (_waypoints.size() > 1 && _transportShip->getDistance(_waypoints[0]) < 100)
	{
		BWAPI::Broodwar->printf("FOLLOW PERIMETER TO SECOND WAYPOINT!");
		//follow perimeter to second waypoint! 
		//clockwise or counterclockwise? 
		int closestPolygonIndex = getClosestVertexIndex(_transportShip);
		UAB_ASSERT_WARNING(closestPolygonIndex != -1, "Couldn't find a closest vertex");

		if (_mapEdgeVertices[(closestPolygonIndex + 1) % _mapEdgeVertices.size()].getApproxDistance(_waypoints[1]) <
			_mapEdgeVertices[(closestPolygonIndex - 1) % _mapEdgeVertices.size()].getApproxDistance(_waypoints[1]))
		{
			BWAPI::Broodwar->printf("FOLLOW clockwise");
			following = 1;
			followPerimeter(following);
		}
		else
		{
			BWAPI::Broodwar->printf("FOLLOW counter clockwise");
			following = -1;
			followPerimeter(following);
		}

	}
	else if (_waypoints.size() > 1 && _transportShip->getDistance(_waypoints[1]) < 50)
	{	
		//if close to second waypoint, go to destination!
		following = 0;
		Micro::SmartMove(_transportShip, to);
	}

}


int TransportManager::getClosestVertexIndex(BWAPI::UnitInterface * unit)
{
	int closestIndex = -1;
	double closestDistance = 10000000;

	for (size_t i(0); i < _mapEdgeVertices.size(); ++i)
	{
		double dist = unit->getDistance(_mapEdgeVertices[i]);
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestIndex = i;
		}
	}

	return closestIndex;
}

int TransportManager::getClosestVertexIndex(BWAPI::Position p)
{
	int closestIndex = -1;
	double closestDistance = 10000000;

	for (size_t i(0); i < _mapEdgeVertices.size(); ++i)
	{
		
		double dist = p.getApproxDistance(_mapEdgeVertices[i]);
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestIndex = i;
		}
	}

	return closestIndex;
}

std::pair<int,int> TransportManager::findSafePath(BWAPI::Position to, BWAPI::Position from)
{
	BWAPI::Broodwar->printf("FROM: [%d,%d]",from.x, from.y);
	BWAPI::Broodwar->printf("TO: [%d,%d]", to.x, to.y);


	//closest map edge point to destination
	int endPolygonIndex = getClosestVertexIndex(to);
	//BWAPI::Broodwar->printf("end indx: [%d]", endPolygonIndex);

	UAB_ASSERT_WARNING(endPolygonIndex != -1, "Couldn't find a closest vertex");
	BWAPI::Position enemyEdge = _mapEdgeVertices[endPolygonIndex];

	BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());
	BWAPI::Position enemyPosition = enemyBaseLocation->getPosition();

	//find the projections on the 4 edges
	UAB_ASSERT_WARNING((_minCorner.isValid() && _maxCorner.isValid()), "Map corners should have been set! (transport mgr)");
	std::vector<BWAPI::Position> p;
	p.push_back(BWAPI::Position(from.x, _minCorner.y));
	p.push_back(BWAPI::Position(from.x, _maxCorner.y));
	p.push_back(BWAPI::Position(_minCorner.x, from.y));
	p.push_back(BWAPI::Position(_maxCorner.x, from.y));

	//for (auto _p : p)
		//BWAPI::Broodwar->printf("p: [%d,%d]", _p.x, _p.y);

	int d1 = p[0].getApproxDistance(enemyPosition);
	int d2 = p[1].getApproxDistance(enemyPosition);
	int d3 = p[2].getApproxDistance(enemyPosition);
	int d4 = p[3].getApproxDistance(enemyPosition);

	//have to choose the two points that are not max or min (the sides!)
	int maxIndex = (d1 > d2 ? d1 : d2) > (d3 > d4 ? d3 : d4) ?
						  (d1 > d2 ? 0 : 1) : (d3 > d4 ? 2 : 3);
	
	

	int minIndex = (d1 < d2 ? d1 : d2) < (d3 < d4 ? d3 : d4) ?
						   (d1 < d2 ? 0 : 1) : (d3 < d4 ? 2 : 3);


	if (maxIndex > minIndex)
	{
		p.erase(p.begin() + maxIndex);
		p.erase(p.begin() + minIndex);
	}
	else
	{
		p.erase(p.begin() + minIndex);
		p.erase(p.begin() + maxIndex);
	}

	//BWAPI::Broodwar->printf("new p: [%d,%d] [%d,%d]", p[0].x, p[0].y, p[1].x, p[1].y);

	//get the one that works best from the two.
	BWAPI::Position waypoint = (enemyEdge.getApproxDistance(p[0]) < enemyEdge.getApproxDistance(p[1])) ? p[0] : p[1];

	int startPolygonIndex = getClosestVertexIndex(waypoint);

	return std::pair<int, int>(startPolygonIndex, endPolygonIndex);

}

BWAPI::Position TransportManager::getFleePosition(int clockwise)
{
	UAB_ASSERT_WARNING(!_mapEdgeVertices.empty(), "We should have a transport route!");

	//BWTA::BaseLocation * enemyBaseLocation = InformationManager::Instance().getMainBaseLocation(BWAPI::Broodwar->enemy());

	// if this is the first flee, we will not have a previous perimeter index
	if (_currentRegionVertexIndex == -1)
	{
		// so return the closest position in the polygon
		int closestPolygonIndex = getClosestVertexIndex(_transportShip);

		UAB_ASSERT_WARNING(closestPolygonIndex != -1, "Couldn't find a closest vertex");

		if (closestPolygonIndex == -1)
		{
			return BWAPI::Position(BWAPI::Broodwar->self()->getStartLocation());
		}
		else
		{
			// set the current index so we know how to iterate if we are still fleeing later
			_currentRegionVertexIndex = closestPolygonIndex;
			return _mapEdgeVertices[closestPolygonIndex];
		}
	}
	// if we are still fleeing from the previous frame, get the next location if we are close enough
	else
	{
		double distanceFromCurrentVertex = _mapEdgeVertices[_currentRegionVertexIndex].getDistance(_transportShip->getPosition());

		// keep going to the next vertex in the perimeter until we get to one we're far enough from to issue another move command
		while (distanceFromCurrentVertex < 128*2)
		{
			_currentRegionVertexIndex = (_currentRegionVertexIndex + clockwise*1) % _mapEdgeVertices.size();

			distanceFromCurrentVertex = _mapEdgeVertices[_currentRegionVertexIndex].getDistance(_transportShip->getPosition());
		}

		return _mapEdgeVertices[_currentRegionVertexIndex];
	}

}

void TransportManager::setTransportShip(BWAPI::UnitInterface * unit)
{
	_transportShip = unit;
}

void TransportManager::setFrom(BWAPI::Position from)
{
	if (from.isValid())
		_from = from;
}
void TransportManager::setTo(BWAPI::Position to)
{
	if (to.isValid())
		_to = to;
}
