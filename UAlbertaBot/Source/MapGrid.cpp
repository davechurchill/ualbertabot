#include "Common.h"
#include "MapGrid.h"

using namespace UAlbertaBot;

MapGrid & MapGrid::Instance() 
{
	static MapGrid instance(BWAPI::Broodwar->mapWidth()*32, BWAPI::Broodwar->mapHeight()*32, Config::Tools::MAP_GRID_SIZE);
	return instance;
}

MapGrid::MapGrid() {}
	
MapGrid::MapGrid(int mapWidth, int mapHeight, int cellSize) 
	: mapWidth(mapWidth)
	, mapHeight(mapHeight)
	, cellSize(cellSize)
	, cols((mapWidth + cellSize - 1) / cellSize)
	, rows((mapHeight + cellSize - 1) / cellSize)
	, cells(rows * cols)
	, lastUpdated(0)
{
	calculateCellCenters();
}

BWAPI::Position MapGrid::getNaturalExpansion() 
{
	return naturalExpansion;
}

BWAPI::Position MapGrid::getLeastExplored() 
{
	int minSeen = 1000000;
	double minSeenDist = 100000;
	int leastRow(0), leastCol(0);

	for (int r=0; r<rows; ++r)
	{
		for (int c=0; c<cols; ++c)
		{
			// get the center of this cell
			BWAPI::Position cellCenter = getCellCenter(r,c);

			// don't worry about places that aren't connected to our start locatin
			if (!BWTA::isConnected(BWAPI::TilePosition(cellCenter), BWAPI::Broodwar->self()->getStartLocation()))
			{
				continue;
			}

			BWAPI::Position home(BWAPI::Broodwar->self()->getStartLocation());
			double dist = home.getDistance(getCellByIndex(r, c).center);
			if ((getCellByIndex(r, c).timeLastVisited < minSeen) || ((getCellByIndex(r, c).timeLastVisited == minSeen) && (dist < minSeenDist)))
			{
				leastRow = r;
				leastCol = c;
				minSeen = getCellByIndex(r, c).timeLastVisited;
				minSeenDist = dist;
			}
		}
	}

	return getCellCenter(leastRow, leastCol);
}

void MapGrid::calculateCellCenters()
{
	for (int r=0; r < rows; ++r)
	{
		for (int c=0; c < cols; ++c)
		{
			GridCell & cell = getCellByIndex(r,c);

			int centerX = (c * cellSize) + (cellSize / 2);
			int centerY = (r * cellSize) + (cellSize / 2);

			// if the x position goes past the end of the map
			if (centerX > mapWidth)
			{
				// when did the last cell start
				int lastCellStart		= c * cellSize;

				// how wide did we go
				int tooWide				= mapWidth - lastCellStart;
				
				// go half the distance between the last start and how wide we were
				centerX = lastCellStart + (tooWide / 2);
			}
			else if (centerX == mapWidth)
			{
				centerX -= 50;
			}

			if (centerY > mapHeight)
			{
				// when did the last cell start
				int lastCellStart		= r * cellSize;

				// how wide did we go
				int tooHigh				= mapHeight - lastCellStart;
				
				// go half the distance between the last start and how wide we were
				centerY = lastCellStart + (tooHigh / 2);
			}
			else if (centerY == mapHeight)
			{
				centerY -= 50;
			}

			cell.center = BWAPI::Position(centerX, centerY);
			assert(cell.center.isValid());
		}
	}
}

BWAPI::Position MapGrid::getCellCenter(int row, int col)
{
	return getCellByIndex(row, col).center;
}

// clear the vectors in the grid
void MapGrid::clearGrid() { 

	for (size_t i(0); i<cells.size(); ++i) 
	{
		cells[i].ourUnits.clear();
		cells[i].oppUnits.clear();
	}
}

// populate the grid with units
void MapGrid::update() 
{
    if (Config::Debug::DrawMapGrid) 
    {
	    for (int i=0; i<cols; i++) 
	    {
	        BWAPI::Broodwar->drawLineMap(i*cellSize, 0, i*cellSize, mapHeight, BWAPI::Colors::Blue);
	    }

	    for (int j=0; j<rows; j++) 
	    {
		    BWAPI::Broodwar->drawLineMap(0, j*cellSize, mapWidth, j*cellSize, BWAPI::Colors::Blue);
	    }

	    for (int r=0; r < rows; ++r)
	    {
		    for (int c=0; c < cols; ++c)
		    {
			    GridCell & cell = getCellByIndex(r,c);
			
			    BWAPI::Broodwar->drawTextMap(cell.center.x, cell.center.y, "Last Seen %d", cell.timeLastVisited);
			    BWAPI::Broodwar->drawTextMap(cell.center.x, cell.center.y+10, "Row/Col (%d, %d)", r, c);
		    }
	    }

    }

	// clear the grid
	clearGrid();

	//BWAPI::Broodwar->printf("MapGrid info: WH(%d, %d)  CS(%d)  RC(%d, %d)  C(%d)", mapWidth, mapHeight, cellSize, rows, cols, cells.size());

	// add our units to the appropriate cell
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->self()->getUnits()) 
	{
		getCell(unit).ourUnits.push_back(unit);
		getCell(unit).timeLastVisited = BWAPI::Broodwar->getFrameCount();
	}

	// add enemy units to the appropriate cell
	for (BWAPI::UnitInterface* unit : BWAPI::Broodwar->enemy()->getUnits()) 
	{
		if (unit->getHitPoints() > 0) 
		{
			getCell(unit).oppUnits.push_back(unit);
			getCell(unit).timeLastOpponentSeen = BWAPI::Broodwar->getFrameCount();
		}
	}
}

void MapGrid::GetUnits(std::vector<BWAPI::UnitInterface *> & units, BWAPI::Position center, int radius, bool ourUnits, bool oppUnits)
{
	const int x0(std::max( (center.x - radius) / cellSize, 0));
	const int x1(std::min( (center.x + radius) / cellSize, cols-1));
	const int y0(std::max( (center.y - radius) / cellSize, 0));
	const int y1(std::min( (center.y + radius) / cellSize, rows-1));
	const int radiusSq(radius * radius);
	for(int y(y0); y<=y1; ++y)
	{
		for(int x(x0); x<=x1; ++x)
		{
			int row = y;
			int col = x;

			GridCell & cell(getCellByIndex(row,col));
			if(ourUnits)
			{
				for (BWAPI::UnitInterface* unit : cell.ourUnits)
				{
					BWAPI::Position d(unit->getPosition() - center);
					if(d.x * d.x + d.y * d.y <= radiusSq)
					{
						if (!contains(units, unit)) 
						{
							units.push_back(unit);
						}
					}
				}
			}
			if(oppUnits)
			{
				for (BWAPI::UnitInterface* unit : cell.oppUnits) if (unit->getType() != BWAPI::UnitTypes::Unknown && unit->isVisible())
				{
					BWAPI::Position d(unit->getPosition() - center);
					if(d.x * d.x + d.y * d.y <= radiusSq)
					{
						if (!contains(units, unit)) 
						{ 
							units.push_back(unit); 
						}
					}
				}
			}
		}
	}
}

bool MapGrid::contains(std::vector<BWAPI::UnitInterface *> & units, BWAPI::UnitInterface* unit) 
{
	for (size_t i(0); i<units.size(); ++i) 
	{
		if (units[i] == unit) 
		{
			return true;
		}
	}

	return false;
}