//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#ifndef BWEM_EXAMPLE_WALL_H
#define BWEM_EXAMPLE_WALL_H

#include <BWAPI.h>
#include "defs.h"
#include <vector>

namespace BWEM
{

class Map;
class Area;
class ChokePoint;

namespace utils
{



//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class ExampleWall
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Given a ChokePoint 'cp', ExampleWall can compute a Terran wall that blocks 'cp'.
// After Possible() is called, Possible() will tell whether a wall was found.
// The possible wall compositions are :
//   - Terran_Barracks
//   - Terran_Barracks, Terran_Supply_Depot
//   - Terran_Barracks, Terran_Supply_Depot, Terran_Supply_Depot
//
// Disclaimer:
//   - Walls computed with tight_t::zealot should be zealot-thight 90% only.
//   - Walls computed with tight_t::zergling should be zergling-thight 60% only.
//   Unfortunatly, reaching better accuracy either involves less walls computed,
//   either requires pixel-level information that is unavailable via the BWAPI library.
//   - The algorithms used in the implementation are very roughly optimized, and quite specific.
//   You should be able to run ExampleWall for each ChokePoint at the intialization,
//   or to run it on a single ChokePoint per frame.
//   For a faster algorithm, or a generic framework, you may want to use some constraint programming approach.
//
//   The current implementation demonstrates the use of Tiles, MiniTiles and altitudes with Areas and ChokePoints.

class ExampleWall
{
public:
	enum class tight_t {zealot, zergling};

								ExampleWall(const Map & theMap, const ChokePoint * cp, tight_t tight = tight_t::zealot);


	const ChokePoint *			GetCP() const							{ return m_cp; }

	void						Compute();
	void						DrawLocations() const;

	bool						Possible() const						{ return !m_Locations.empty(); }
	int							Size() const							{ return m_Locations.size(); }
	BWAPI::Position				Center() const							{ return m_center; }

	const std::vector<BWAPI::TilePosition> &	Locations() const		{ return m_Locations; }
	const std::vector<BWAPI::UnitType> &		BuildingTypes() const	{ return m_BuildingTypes; }

protected:
	void						Compute(int wallSize, const std::vector<BWAPI::TilePosition> & BuildableBorderTiles1,
													  const std::vector<BWAPI::TilePosition> & BuildableBorderTiles2);
	void						Compute(const Area * area);
	void						PostCompute();

	const Map *							m_theMap;
	const ChokePoint *					m_cp;
	tight_t								m_tight;
	BWAPI::Position						m_center = BWAPI::Positions::None;
	std::vector<BWAPI::TilePosition>	m_Locations;
	std::vector<BWAPI::UnitType>		m_BuildingTypes;
};


std::vector<ExampleWall> findWalls(const Map & theMap);

//  To prints the computed walls each frame, just add these two lines in your onFrame() handler.
//
// 	for (const ExampleWall & wall : findWalls(Map::Instance()))
//		wall.DrawLocations();
//
//  However, this will slow down a lot the game, so you may want to store the returned vector
//  in your onStart() handler and then use it in your onFrame() handler.



}} // namespace BWEM::utils


#endif

