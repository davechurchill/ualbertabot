//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "exampleWall.h"
#include "map.h"
#include "base.h"
#include "neutral.h"
#include "mapDrawer.h"
#include "bwapiExt.h"
#include <numeric>

using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace utils;
using namespace BWAPI_ext;

namespace utils
{

static bool canBuildWall(const Map & theMap, BWAPI::UnitType type, TilePosition location)
{
	TilePosition dim(type.tileSize());

	if (!theMap.Valid(location)) return false;
	if (!theMap.Valid(location + dim - 1)) return false;

	for (int dy = 0 ; dy < dim.y ; ++dy)
	for (int dx = 0 ; dx < dim.x ; ++dx)
	{
		TilePosition t = location + TilePosition(dx, dy);

		const auto & tile = theMap.GetTile(t);
		
		if (!tile.Buildable()) return false;
		if (tile.GetNeutral()) return false;
	}

	return true;
}


// Returns a small list of buildable Tiles in the seaside (minimal altitude) of 'area',
// starting at the ChokePoint end 'cpEnd'.
// 'area' should be one of the two Areas of the ChokePoint (the intent is to restrict the Tiles to one of these areas).
// If 'area' == nullptr, any area's seaside can be searched, allowing the Tiles to cross the ChokePoint.

static vector<TilePosition> findBuildableBorderTiles(const Map & theMap, WalkPosition cpEnd, const Area * area)
{
	vector<TilePosition> BuildableBorderTiles;

	// Although we want Tiles, we need to use MiniTiles for accuracy.
	vector<WalkPosition> Visited;
	queue<WalkPosition> ToVisit;

	ToVisit.push(cpEnd);
	Visited.push_back(cpEnd);
	int seasideCount = 0;

	while (!ToVisit.empty())
	{
		WalkPosition current = ToVisit.front();
		ToVisit.pop();
		for (WalkPosition delta : {	WalkPosition(-1, -1), WalkPosition(0, -1), WalkPosition(+1, -1),
									WalkPosition(-1,  0),					   WalkPosition(+1,  0),
									WalkPosition(-1, +1), WalkPosition(0, +1), WalkPosition(+1, +1)})
		{
			WalkPosition next = current + delta;
			if (theMap.Valid(next))
				if (!contains(Visited, next))
				{
					const MiniTile & Next = theMap.GetMiniTile(next, check_t::no_check); 
					const Tile & NextTile = theMap.GetTile(TilePosition(next), check_t::no_check); 

					const bool seaside = (Next.Altitude() <= (seasideCount <= 8 ? 24 : 11)) &&
									(area ? Next.AreaId() == area->Id() : Next.AreaId() > 0);
					if (seaside || NextTile.GetNeutral())
					{
						ToVisit.push(next);
						Visited.push_back(next);
						if (seaside) ++seasideCount;
						if (seasideCount > (area ? 130 : 260)) return BuildableBorderTiles;

						// Uncomment this to see the visited MiniTiles
					///	bw->drawBoxMap(Position(next), Position(next) + 8, Colors::White);

						if (NextTile.Buildable() && !NextTile.GetNeutral() && (Next.Altitude() <= 11))
						{
							if (!contains(BuildableBorderTiles, TilePosition(next)))
								BuildableBorderTiles.push_back(TilePosition(next));

							if ((BuildableBorderTiles.size() >= 3) && (roundedDist(TilePosition(next), TilePosition(cpEnd)) > 12))
								return BuildableBorderTiles;
						}
					}
				}
		}
	}

	return BuildableBorderTiles;
}


class BorderTileInfo
{
public:
					BorderTileInfo(const Map & theMap, const TilePosition & t)
					:
						m_topBorder(minAltitudeTop(t, theMap) <= 11),
						m_bottomBorder(minAltitudeBottom(t, theMap) <= 11),
						m_leftBorder(minAltitudeLeft(t, theMap) <= 11),
						m_rightBorder(minAltitudeRight(t, theMap) <= 11)
					{}
	
	bool			TopBorder() const		{ return m_topBorder; }
	bool			BottomBorder() const	{ return m_bottomBorder; }
	bool			LeftBorder() const		{ return m_leftBorder; }
	bool			RightBorder() const		{ return m_rightBorder; }

private:
	bool m_topBorder;
	bool m_bottomBorder;
	bool m_leftBorder;
	bool m_rightBorder;
};


static bool tightEnough(UnitType type, const BorderTileInfo & BorderTile)
{
	if (type == Terran_Barracks)
	{
		if (BorderTile.TopBorder()) return true;
		if (BorderTile.RightBorder()) return true;
		if (BorderTile.BottomBorder() && BorderTile.LeftBorder()) return true;
		return false;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class ExampleWall
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

ExampleWall::ExampleWall(const Map & theMap, const ChokePoint * cp, tight_t tight)
: m_theMap(&theMap), m_cp(cp), m_tight(tight)
{
}


void ExampleWall::DrawLocations() const
{
	const BWAPI::Color colorWall = BWAPI::Colors::Purple;

	for (int i = 0 ; i < (int)m_Locations.size() ; ++i)
	{
		TilePosition pos = m_Locations[i];
		TilePosition dim = m_BuildingTypes[i].tileSize();
		
		for (int n = 0 ; n < 2 ; ++n)
			bw->drawBoxMap(Position(pos) + n, Position(pos + dim) - n, colorWall);
	}
}


void ExampleWall::Compute(int wallSize, const vector<TilePosition> & BuildableBorderTiles1,
										const vector<TilePosition> & BuildableBorderTiles2)
{
	map<TilePosition, BorderTileInfo> map_BorderTile_Info;
	for (const TilePosition & t : BuildableBorderTiles1) map_BorderTile_Info.emplace(t, BorderTileInfo(*m_theMap, t));
	for (const TilePosition & t : BuildableBorderTiles2) map_BorderTile_Info.emplace(t, BorderTileInfo(*m_theMap, t));

	TilePosition dimDepot(UnitType(Terran_Supply_Depot).tileSize());
	TilePosition dimBarrack(UnitType(Terran_Barracks).tileSize());

	m_BuildingTypes = {Terran_Barracks, Terran_Supply_Depot, Terran_Supply_Depot};
	m_BuildingTypes.resize(wallSize);

	vector<TilePosition> BuildingDims = {dimBarrack, dimDepot, dimDepot};
	BuildingDims.resize(wallSize);


	TilePosition wallMaxDim = accumulate(BuildingDims.begin(), BuildingDims.end(), TilePosition(0, 0));

	for (const TilePosition & borderTile1 : BuildableBorderTiles1)
	for (const TilePosition & borderTile2 : BuildableBorderTiles2)
		if (abs(borderTile1.y - borderTile2.y) < wallMaxDim.y)
		if (abs(borderTile1.x - borderTile2.x) < wallMaxDim.x)
			for (int permutation = 0 ; permutation < wallSize ;
					++permutation,
					// left rotate for next permutation
					rotate(m_BuildingTypes.begin(), m_BuildingTypes.begin()+1, m_BuildingTypes.end()),
					rotate(BuildingDims.begin(), BuildingDims.begin()+1, BuildingDims.end())
				)
			{
				if (!tightEnough(m_BuildingTypes.front(), map_BorderTile_Info.at(borderTile1))) continue;
				if (!tightEnough(m_BuildingTypes.back(), map_BorderTile_Info.at(borderTile2))) continue;

				for (int dy1 = 0 ; dy1 < BuildingDims.front().y ; ++dy1)
				for (int dx1 = 0 ; dx1 < BuildingDims.front().x ; ++dx1)
				for (int dy2 = 0 ; dy2 < BuildingDims.back().y ; ++dy2)
				for (int dx2 = 0 ; dx2 < BuildingDims.back().x ; ++dx2)
				{
					if ((wallSize == 1) && ((dx1 != dx2) || (dy1 != dy2))) continue;

					m_Locations.resize(wallSize);
					m_Locations.front() = borderTile1 - TilePosition(dx1, dy1);
					m_Locations.back()  = borderTile2 - TilePosition(dx2, dy2);

					if (wallSize == 1)
						if (!overlap(m_Locations.front(), BuildingDims.front(), borderTile1, TilePosition(1, 1)))
							continue;

					if (!canBuildWall(*m_theMap, m_BuildingTypes.front(), m_Locations.front())) continue;

					if (wallSize >= 2)
					{
						if (!canBuildWall(*m_theMap, m_BuildingTypes.back(),  m_Locations.back()))  continue;

						if (overlap(m_Locations.front(), BuildingDims.front(),
									m_Locations.back(), BuildingDims.back())) continue;

						if (wallSize == 2)
							if (disjoint(m_Locations.front(), BuildingDims.front(),
										 m_Locations.back(), BuildingDims.back())) continue;
					}

					int minX = (wallSize < 3) ? -1 : min(m_Locations.front().x, m_Locations.back().x) - BuildingDims[1].x;
					int minY = (wallSize < 3) ? -1 : min(m_Locations.front().y, m_Locations.back().y) - BuildingDims[1].y;
					int maxX = (wallSize < 3) ? -1 : max(m_Locations.front().x + BuildingDims.front().x, m_Locations.back().x + BuildingDims.back().x);
					int maxY = (wallSize < 3) ? -1 : max(m_Locations.front().y + BuildingDims.front().y, m_Locations.back().y + BuildingDims.back().y);

					for (int y = minY ; y <= maxY ; ++y)
					for (int x = minX ; x <= maxX ; ++x)
					{
						if (wallSize == 3)
						{
							m_Locations[1] = TilePosition(x, y);

							bool middleBuildingHasBorderTiles = false;
							for (const auto & BuildableBorderTiles : {BuildableBorderTiles1, BuildableBorderTiles2})
								for (const TilePosition & t : BuildableBorderTiles)
									if (overlap(m_Locations[1], BuildingDims[1], t, TilePosition(1, 1)))
										middleBuildingHasBorderTiles = true;
							if (middleBuildingHasBorderTiles) continue;

							if (!canBuildWall(*m_theMap, m_BuildingTypes[1], m_Locations[1])) continue;

							if (overlap(m_Locations[1], BuildingDims[1], m_Locations[0], BuildingDims[0])) continue;
							if (overlap(m_Locations[1], BuildingDims[1], m_Locations[2], BuildingDims[2])) continue;

							if (disjoint(m_Locations[1], BuildingDims[1], m_Locations[0], BuildingDims[0])) continue;
							if (disjoint(m_Locations[1], BuildingDims[1], m_Locations[2], BuildingDims[2])) continue;
						}

						bool zealotTight = true;
						for (int b = 0 ; b < wallSize ; ++b) if (m_BuildingTypes[b] == Terran_Barracks)
						for (int d = 0 ; d < wallSize ; ++d) if (m_BuildingTypes[d] == Terran_Supply_Depot)
							if (!disjoint(m_Locations[b], BuildingDims[b], m_Locations[d], BuildingDims[d]))
							{
								if (m_Locations[b].y + BuildingDims[b].y == m_Locations[d].y)
									zealotTight = false;
								if (m_Locations[d].x + BuildingDims[d].x == m_Locations[b].x)
									zealotTight = false;
							}
						if (!zealotTight) continue;

						if (m_tight == tight_t::zergling)
						{
							bool zerglingTight = true;
							for (int b = 0 ; b < wallSize ; ++b) if (m_BuildingTypes[b] == Terran_Barracks)
							for (int d = 0 ; d < wallSize ; ++d) if (m_BuildingTypes[d] == Terran_Supply_Depot)
								if (!disjoint(m_Locations[b], BuildingDims[b], m_Locations[d], BuildingDims[d]))
								{
									if (m_Locations[b].y + BuildingDims[b].y == m_Locations[d].y)
										zerglingTight = false;
									if (m_Locations[d].x + BuildingDims[d].x == m_Locations[b].x)
										zerglingTight = false;
									if (m_Locations[b].x + BuildingDims[b].x == m_Locations[d].x)
										zerglingTight = false;
								}
							for (int d1 = 0    ; d1 < wallSize ; ++d1) if (m_BuildingTypes[d1] == Terran_Supply_Depot)
							for (int d2 = d1+1 ; d2 < wallSize ; ++d2) if (m_BuildingTypes[d2] == Terran_Supply_Depot)
								if (!disjoint(m_Locations[d1], BuildingDims[d1], m_Locations[d2], BuildingDims[d2]))
								{
									if (m_Locations[d1].x + BuildingDims[d1].x == m_Locations[d2].x)
										zerglingTight = false;
									if (m_Locations[d2].x + BuildingDims[d2].x == m_Locations[d1].x)
										zerglingTight = false;
								}
							if (!zerglingTight) continue;
						}

						const altitude_t maxAltitudeOfCP = m_theMap->GetMiniTile(m_cp->Center()).Altitude();
						const int lengthCP = 2*maxAltitudeOfCP;

						bool blockingDepot = false;
						if (lengthCP < 5*32)
							for (int d = 0 ; d < wallSize ; ++d)
								if (m_BuildingTypes[d] == Terran_Supply_Depot)
								{
									Position centerDepot = Position(m_Locations[d]) + Position(BuildingDims[d])/2;
									if (roundedDist(centerDepot, center(m_cp->Center())) < 2*32)
										blockingDepot = true;
								}
						if (blockingDepot) continue;


						// left rotate until back to first permutation
						while (permutation++ < wallSize)
						{
							rotate(m_Locations.begin(), m_Locations.begin()+1, m_Locations.end());
							rotate(m_BuildingTypes.begin(), m_BuildingTypes.begin()+1, m_BuildingTypes.end());
						}

						return;

					}

				}
			}

	m_Locations.clear();
}


// Try to find a wall inside 'area' that blocks the ChokePoint m_cp.
// If 'area' != nullptr, 'area' must be one of the two Areas of m_cp.
// If 'area' == nullptr, the wall may cross the ChokePoint.
void ExampleWall::Compute(const Area * area)
{
	// Since the wall must block m_cp, it must touch the seasides of 'area' next to both ends of the ChokePoint m_cp.
	// So, for each end of m_cp, we first retrieve a list of candidates buildable, border Tiles for the Wall:

	vector<TilePosition> BuildableBorderTiles1 = findBuildableBorderTiles(*m_theMap, m_cp->Pos(ChokePoint::end1), area);
	vector<TilePosition> BuildableBorderTiles2 = findBuildableBorderTiles(*m_theMap, m_cp->Pos(ChokePoint::end2), area);

	// Uncomment this to see the visited retrieved Tiles
///	for (auto t : BuildableBorderTiles1) bw->drawBoxMap(Position(t), Position(t) + 32, Colors::Orange);
///	for (auto t : BuildableBorderTiles2) bw->drawBoxMap(Position(t), Position(t) + 32, Colors::Orange);

	// Prefer a small wall rather than a bigger one:
	for (int wallSize = 1 ; wallSize <= 3 ; ++wallSize)
	{
		Compute(wallSize, BuildableBorderTiles1, BuildableBorderTiles2);
		if (Possible())
			break;

	}
}


void ExampleWall::Compute()
{
	// First, look for a wall in m_cp->GetAreas().first only.
	// If not found, look for a wall in m_cp->GetAreas().second only.
	// If not found, look for a wall in both areas.

	// We could easily use a better strategy here.

	if (!Possible()) Compute(m_cp->GetAreas().first);
	if (!Possible()) Compute(m_cp->GetAreas().second);
	if (!Possible()) Compute(nullptr);

	PostCompute();
}


void ExampleWall::PostCompute()
{
	if (Possible())
	{
		m_center = {0, 0};
		for (int i = 0 ; i < (int)m_Locations.size() ; ++i)
			m_center += Position(m_Locations[i]) + Position(m_BuildingTypes[i].tileSize())/2;

		m_center /= m_Locations.size();
	}
}



vector<ExampleWall> findWalls(const Map & theMap)
{
	vector<ExampleWall> Walls;

	for (const Area & area : theMap.Areas())
		for (const ChokePoint * cp : area.ChokePoints())
		{
			Walls.emplace_back(theMap, cp);
			Walls.back().Compute();
		}

	return Walls;
}


}} // namespace BWEM::utils



