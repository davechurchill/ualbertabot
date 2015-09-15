#pragma once

#include <vector>
#include "BWAPI.h"

namespace UAlbertaBot
{
class DistanceMap
{
	int					rows,
						cols,
						startRow,
						startCol;

	std::vector<int>	dist;
	std::vector<char>	moveTo;
    std::vector<BWAPI::TilePosition>    sorted;

	int getIndex(const int row, const int col) const
	{
		return row * cols + col;
	}

	int getIndex(const BWAPI::Position & p) const
	{
		return getIndex(p.y / 32, p.x / 32);
	}

public:

	DistanceMap () 
		: dist(std::vector<int>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), -1))
		, moveTo(std::vector<char>(BWAPI::Broodwar->mapWidth() * BWAPI::Broodwar->mapHeight(), 'X'))
		, rows(BWAPI::Broodwar->mapHeight()), cols(BWAPI::Broodwar->mapWidth()), startRow(-1), startCol(-1) 
	{
		//BWAPI::Broodwar->printf("New Distance Map With Dimensions (%d, %d)", rows, cols);
	}

	int & operator [] (const int index)						{ return dist[index]; }
	int & operator [] (const BWAPI::Position & pos)			{ return dist[getIndex(pos.y / 32, pos.x / 32)]; }
	void setMoveTo(const int index, const char val)			{ moveTo[index] = val; }
	void setDistance(const int index, const int val)		{ dist[index] = val; }
	void setStartPosition(const int sr, const int sc)		{ startRow = sr; startCol = sc; }

	// reset the distance map
	void reset(const int & rows, const int & cols)
	{
		this->rows = rows;
		this->cols = cols;
		dist = std::vector<int>(rows * cols, -1);
        sorted.clear();
		moveTo = std::vector<char>(rows * cols, 'X');
		startRow = -1;
		startCol = -1;
	}

    const std::vector<BWAPI::TilePosition> & getSortedTiles() const
    {
        return sorted;
    }

	// reset the distance map
	void reset()
	{
		std::fill(dist.begin(), dist.end(), -1);
		std::fill(moveTo.begin(), moveTo.end(), 'X');
        sorted.clear();
		startRow = -1;
		startCol = -1;
	}

	bool isConnected(const BWAPI::Position p) const
	{
		return dist[getIndex(p)] != -1;
	}

    void addSorted(const BWAPI::TilePosition & tp)
    {
        sorted.push_back(tp);
    }

	// given a position, get the position we should move to to minimize distance
	BWAPI::Position getMoveTo(const BWAPI::Position p, const int lookAhead = 1) const
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
};
}