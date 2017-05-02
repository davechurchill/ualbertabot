#include "DistanceMap.h"

using namespace UAlbertaBot;

DistanceMap::DistanceMap() 
	: dist(std::vector<int>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), -1))
	, moveTo(std::vector<char>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), 'X'))
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
	return getIndex(p.y / 32, p.x / 32);
}

int & DistanceMap::operator [] (const int index)
{ 
    return dist[index]; 
}

int & DistanceMap::operator [] (const BWAPI::Position & pos)
{ 
    return dist[getIndex(pos.y / 32, pos.x / 32)]; 
}

void DistanceMap::setMoveTo(const int index, const char val)
{ 
    moveTo[index] = val; 
}

void DistanceMap::setDistance(const int index, const int val)
{ 
    dist[index] = val; 
}

void DistanceMap::setStartPosition(const int sr, const int sc)
{ 
    _startRow = sr; 
    _startCol = sc; 
}

const std::vector<BWAPI::TilePosition> & DistanceMap::getSortedTiles() const
{
    return sorted;
}

bool DistanceMap::isConnected(const BWAPI::Position p) const
{
	return dist[getIndex(p)] != -1;
}

void DistanceMap::addSorted(const BWAPI::TilePosition & tp)
{
    sorted.push_back(tp);
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
		if (moveTo[index] == 'L')
		{
			col -= 1;
		} 
		else if (moveTo[index] == 'R')
		{
			col += 1;
		} 
		else if (moveTo[index] == 'U')
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