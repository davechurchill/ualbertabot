//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "tiles.h"
#include "neutral.h"
#include "bwapiExt.h"
#include "winutils.h"
#include "map.h"


using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace detail;
using namespace BWAPI_ext;

namespace {	enum {sizeof_Tile = sizeof(Tile), sizeof_MiniTile = sizeof(MiniTile)}; }

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class MiniTile
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

const Area::id MiniTile::blockingCP = std::numeric_limits<Area::id>::min();


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class Tile
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


int Tile::StackedNeutrals() const
{
	int stackSize = 0;
	for (Neutral * pStacked = GetNeutral() ; pStacked ; pStacked = pStacked->NextStacked())
		++stackSize;

	return stackSize;
};



altitude_t minAltitudeTop(const TilePosition & tile, const Map & theMap)
{
	WalkPosition w(tile);

	return min(theMap.GetMiniTile(w + WalkPosition(1, 0), utils::check_t::no_check).Altitude(),
			   theMap.GetMiniTile(w + WalkPosition(2, 0), utils::check_t::no_check).Altitude());
}


altitude_t minAltitudeBottom(const TilePosition & tile, const Map & theMap)
{
	WalkPosition w(tile);

	return min(theMap.GetMiniTile(w + WalkPosition(1, 3), utils::check_t::no_check).Altitude(),
			   theMap.GetMiniTile(w + WalkPosition(2, 3), utils::check_t::no_check).Altitude());
}


altitude_t minAltitudeLeft(const TilePosition & tile, const Map & theMap)
{
	WalkPosition w(tile);

	return min(theMap.GetMiniTile(w + WalkPosition(0, 1), utils::check_t::no_check).Altitude(),
			   theMap.GetMiniTile(w + WalkPosition(0, 2), utils::check_t::no_check).Altitude());
}


altitude_t minAltitudeRight(const TilePosition & tile, const Map & theMap)
{
	WalkPosition w(tile);

	return min(theMap.GetMiniTile(w + WalkPosition(3, 1), utils::check_t::no_check).Altitude(),
			   theMap.GetMiniTile(w + WalkPosition(3, 2), utils::check_t::no_check).Altitude());
}


} // namespace BWEM



