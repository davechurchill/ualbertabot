#include "HLSquad.h"

using namespace UAlbertaBot;

HLSquadOrder::HLSquadOrder()
	: _type(None)
{
}

HLSquadOrder::HLSquadOrder(OrderType type, BWTA::Region* target)
	: _type(type)
	, _target(target)
{
}

HLSquad::HLSquad(const std::vector<UnitInfo> & units, BWTA::Region *region) :
_currentRegion(region),
_framesTravelled(0),
//_units(units.begin(),units.end()),
_order(),
_speed(std::min_element(units.begin(), units.end(), [](const UnitInfo &u1, const UnitInfo &u2)
{
	return u1.type.topSpeed() < u2.type.topSpeed();
})->type.topSpeed()),
_strengthCalculated(false)
{
	for (const auto &unit : units)
	{
		_units[unit.unitID] = unit;
	}

	UAB_ASSERT(_currentRegion != NULL, "Current region is null!");
}

void HLSquad::addUnit(const UnitInfo &unit)
{
	_speed = std::min(_speed, unit.type.topSpeed());
	_strengthCalculated = false;
	_units[unit.unitID]=unit;
}

int HLSquad::travel(int frames)
{
	_framesTravelled += frames;

	if (!_path.empty())
	{
		int framesToNext;
		while (!_path.empty() && 
			(_framesTravelled > 
			(framesToNext = _currentRegion->getCenter().getDistance(_path.front()->getCenter()) / _speed)))
		{
			_currentRegion = _path.front();
			UAB_ASSERT(_currentRegion != NULL, "Current region is null!");
			_path.pop_front();
			_framesTravelled -= framesToNext;
		}
	}

	if (_path.empty())//reached destination
	{
		int remainingFrames = _framesTravelled;
		_framesTravelled = 0;
		return remainingFrames;
	}
	return 0;
}

void HLSquad::order(HLSquadOrder order)
{
	if (order._type == HLSquadOrder::Attack || order._type == HLSquadOrder::Defend)
	{
		UAB_ASSERT(_currentRegion != NULL, "Current region is null!");
		_path = getPath(_currentRegion, order._target);
	}
	else if (order._type ==HLSquadOrder::None)
	{
		_path.clear();
	}
	_order = order;
	_framesTravelled = 0;

}
int HLSquad::getStrength() const
{
	if (!_strengthCalculated)
	{
		calculateStrength();
	}
	return _strength;
}
int HLSquad::getGroundStrength() const
{
	if (!_strengthCalculated)
	{
		calculateStrength();
	}
	return _groundStrength;
}
int HLSquad::getAntiAirStrength() const
{
	if (!_strengthCalculated)
	{
		calculateStrength();
	}
	return _antiAirStrength;
}
int HLSquad::getFlyingStrength() const
{
	if (!_strengthCalculated)
	{
		calculateStrength();
	}
	return _flyingStrength;
}
void HLSquad::calculateStrength() const
{
	_groundStrength = _flyingStrength = _antiAirStrength = _strength = 0;
	for (auto u : _units)
	{
		if (u.second.type.airWeapon() != BWAPI::WeaponTypes::None)
		{
			_antiAirStrength++;
		}
		if (u.second.type.groundWeapon() != BWAPI::WeaponTypes::None)
		{
			_groundStrength++;
		}
		if (u.second.type.airWeapon() != BWAPI::WeaponTypes::None || u.second.type.groundWeapon() != BWAPI::WeaponTypes::None)
		{
			if (u.second.isFlyer())
			{
				_flyingStrength++;
			}
			_strength++;
		}
	}
}

//class Node{
//	const BWTA::Region* region;
//	int gscore, hscore;
//public:
//	int f_score() const{ return gscore + hscore; }
//	int g_score() const{ return gscore; }
//	Node(const BWTA::Region* r, int g, int h) :region(r), gscore(g), hscore(h){}
//	const BWTA::Region* getRegion() const{ return region; }
//	bool operator==(const Node &other){ return this->region == other.region; }
//};
struct NodeComp{
	const std::unordered_map<BWTA::Region *, int> &_g_score, &_f_score;
	NodeComp(const std::unordered_map<BWTA::Region *, int> &g_score, const std::unordered_map<BWTA::Region *, int> &f_score) :
		_g_score(g_score), _f_score(f_score){}
	bool operator()(BWTA::Region *r1, BWTA::Region *r2) const{
		return _f_score.at(r1) < _f_score.at(r2);
	};
};
//namespace std{
//	template<>struct hash < Node > {
//		size_t operator()(const Node &n){
//			return std::hash<const BWTA::Region*>()(n.getRegion());
//		}
//	};
//}

std::list<BWTA::Region*> reconstruct_path(const std::unordered_map<BWTA::Region *, BWTA::Region *> &came_from, BWTA::Region *current)
{
	std::list<BWTA::Region*> total_path;
	total_path.push_front(current);
	while (came_from.count(current) > 0)
	{
		current = came_from.at(current);
		total_path.push_front(current);
	}
	return total_path;
}
std::list<BWTA::Region*> UAlbertaBot::getPath(BWTA::Region *from, BWTA::Region *to)
{
	//Logger::LogAppendToFile(UAB_LOGFILE, "Performing A*");
	
	std::unordered_set <BWTA::Region *> closed;
	std::unordered_set <BWTA::Region *> open;
	std::vector<BWTA::Region *> open_queue;
	std::unordered_map<BWTA::Region *, BWTA::Region *> came_from;
	std::unordered_map<BWTA::Region *, int> g_score;
	std::unordered_map<BWTA::Region *, int> f_score;
	NodeComp comp(g_score,f_score);
	open.insert(from);
	open_queue.push_back(from);
	g_score[from] = 0;
	f_score[from] = g_score[from] + from->getCenter().getApproxDistance(to->getCenter());
	std::make_heap(open_queue.begin(), open_queue.end(), comp);
	



	//while openset is not empty
	while (!open_queue.empty())
	{
		//current : = the node in openset having the lowest f_score[] value
		auto current = open_queue.front();


		//if current = goal
		if (current == to)
		{
			return reconstruct_path(came_from, to);
		}
		
		//	remove current from openset
		std::pop_heap(open_queue.begin(), open_queue.end(), comp);
		open_queue.pop_back();
		open.erase(current);
	
		
		//	for each neighbor in neighbor_nodes(current)
		for (auto n : getNeighbours(current))
		{
			//		if neighbor in closedset
			if (closed.find(n) != closed.end())
			{
				continue;
			}
			//tentative_g_score: = g_score[current] + dist_between(current, neighbor)
			int tentative_g_score = g_score.at(current) + current->getCenter().getApproxDistance(n->getCenter());


			//							if neighbor not in openset or tentative_g_score < g_score[neighbor]
			auto stored = open.find(n);
			if (stored == open.end() || tentative_g_score < g_score.at(n))
			{
				//								came_from[neighbor] : = current
				came_from[n] = current;
				//								g_score[neighbor] : = tentative_g_score
				g_score[n] = tentative_g_score;
				//								f_score[neighbor] : = g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
				f_score[n] = g_score.at(n) + n->getCenter().getApproxDistance(to->getCenter());
				//								if neighbor not in openset
				if (stored == open.end())
					//									add neighbor to openset
				{
					open.insert(n);
					open_queue.push_back(n);
					std::push_heap(open_queue.begin(), open_queue.end(), comp);
				}
				else//value updated, need to re-sort
				{
					std::make_heap(open_queue.begin(), open_queue.end(), comp);
				}
			}
		}
		//	add current to closedset
		closed.insert(current);

	}
}

std::unordered_set<BWTA::Region*> UAlbertaBot::getNeighbours(const BWTA::Region *region)
{
	std::unordered_set<BWTA::Region*> neighbours;
	for (auto c : region->getChokepoints())
	{
		if (c->getRegions().first != region)
		{
			neighbours.insert(c->getRegions().first);
		}
		else
		{
			neighbours.insert(c->getRegions().second);
		}
	}
	return neighbours;
}


std::string HLSquad::toString() const
{
	std::stringstream ss;

	ss << "Squad at " << _currentRegion->getCenter() << ", Units: ";
	for (auto u : _units)
	{
		ss << u.second.type.getName() << " ";
	}


	return ss.str();
}