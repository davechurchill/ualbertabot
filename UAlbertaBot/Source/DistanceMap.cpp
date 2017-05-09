#include "DistanceMap.h"
#include "UABAssert.h"

using namespace UAlbertaBot;

DistanceMap::DistanceMap() 
	: _dist(std::vector<int>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), -1))
	, _moveTo(std::vector<char>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), 'X'))
	, _rows(BWAPI::Broodwar->mapHeight()), _cols(BWAPI::Broodwar->mapWidth()), _startRow(-1), _startCol(-1) 
{
	//BWAPI::Broodwar->printf("New Distance Map With Dimensions (%d, %d)", _rows, _cols);
}

int DistanceMap::getIndex(const int row, const int col) const
{
	return row * _cols + col;
}

int DistanceMap::getIndex(const BWAPI::Position & p) const
{
    UAB_ASSERT(p.isValid(), "Getting the Index of a position not in the map bounds: Position = (%d, %d)", p.x, p.y);
    
	return getIndex(p.y / 32, p.x / 32);
}

const int & DistanceMap::operator [] (const int index) const
{ 
    UAB_ASSERT(index < _dist.size(), "Index out of range: Index = %d, Size = %d", index, _dist.size());

    return _dist[index]; 
}

int & DistanceMap::operator [] (const int index)
{ 
    UAB_ASSERT(index < _dist.size(), "Index out of range: Index = %d, Size = %d", index, _dist.size());

    return _dist[index]; 
}

const int & DistanceMap::operator [] (const BWAPI::Position & pos) const
{ 
    return (*this)[getIndex(pos)]; 
}

void DistanceMap::setMoveTo(const int index, const char val)
{ 
    UAB_ASSERT(index < _dist.size(), "Index out of range: Index = %d, Size = %d", index, _dist.size());

    _moveTo[index] = val; 
}

void DistanceMap::setDistance(const int index, const int val)
{ 
    UAB_ASSERT(index < _dist.size(), "Index out of range: Index = %d, Size = %d", index, _dist.size());

    _dist[index] = val; 
}

void DistanceMap::setStartPosition(const int sr, const int sc)
{ 
    _startRow = sr; 
    _startCol = sc; 
}

const std::vector<BWAPI::TilePosition> & DistanceMap::getSortedTiles() const
{
    return _sorted;
}

bool DistanceMap::isConnected(const BWAPI::Position p) const
{
	return (*this)[p] != -1;
}

void DistanceMap::addSorted(const BWAPI::TilePosition & tp)
{
    _sorted.push_back(tp);
}

// given a position, get the position we should move to to minimize distance
BWAPI::Position DistanceMap::getMoveTo(const BWAPI::Position p, const int lookAhead) const
{
	// the initial row an column
	int row = p.y / 32;
	int col = p.x / 32;
		
	// for each lookahead
	for (int i=0; i<lookAhead; ++i)
	{
		// get the index
		int index = getIndex(row,col);

		// adjust the row and column accordingly
		if (_moveTo[index] == 'L')
		{
			col -= 1;
		} 
		else if (_moveTo[index] == 'R')
		{
			col += 1;
		} 
		else if (_moveTo[index] == 'U')
		{
			row -= 1;
		} 
		else
		{
			row += 1;
		}
	}

	// return the position
	return BWAPI::Position(col * 32 + 16, row * 32 + 16);
}