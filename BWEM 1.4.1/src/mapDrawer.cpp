//////////////////////////////////////////////////////////////////////////
//
// This file is part of the BWEM Library.
// BWEM is free software, licensed under the MIT/X11 License. 
// A copy of the license is provided with the library in the LICENSE file.
// Copyright (c) 2015, 2017, Igor Dimitrijevic
//
//////////////////////////////////////////////////////////////////////////


#include "mapDrawer.h"
#include "map.h"

using namespace BWAPI;
using namespace BWAPI::UnitTypes::Enum;
namespace { auto & bw = Broodwar; }

using namespace std;


namespace BWEM {

using namespace BWAPI_ext;

namespace utils {


//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
//                                  class MapDrawer
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////

bool MapDrawer::showSeas						= true;
bool MapDrawer::showLakes						= true;
bool MapDrawer::showUnbuildable					= true;
bool MapDrawer::showGroundHeight				= true;
bool MapDrawer::showMinerals					= true;
bool MapDrawer::showGeysers						= true;
bool MapDrawer::showStaticBuildings				= true;
bool MapDrawer::showBases						= true;
bool MapDrawer::showAssignedRessources			= true;
bool MapDrawer::showFrontier					= true;
bool MapDrawer::showCP							= true;

const Color MapDrawer::Color::sea					= BWAPI::Colors::Blue;
const Color MapDrawer::Color::lakes					= BWAPI::Colors::Blue;
const Color MapDrawer::Color::unbuildable			= BWAPI::Colors::Brown;
const Color MapDrawer::Color::highGround			= BWAPI::Colors::Brown;
const Color MapDrawer::Color::veryHighGround		= BWAPI::Colors::Red;
const Color MapDrawer::Color::minerals				= BWAPI::Colors::Cyan;
const Color MapDrawer::Color::geysers				= BWAPI::Colors::Green;
const Color MapDrawer::Color::staticBuildings		= BWAPI::Colors::Purple;
const Color MapDrawer::Color::bases					= BWAPI::Colors::Blue;
const Color MapDrawer::Color::assignedRessources	= BWAPI::Colors::Blue;
const Color MapDrawer::Color::frontier				= BWAPI::Colors::Grey;
const Color MapDrawer::Color::cp					= BWAPI::Colors::White;


bool MapDrawer::ProcessCommandVariants(const string & command, const string & attributName, bool & attribut)
{
	if (command == "show " + attributName) { attribut = true; return true; }
	if (command == "hide " + attributName) { attribut = false; return true; }
	if (command == attributName)		   { attribut = !attribut; return true; }
	return false;
}


bool MapDrawer::ProcessCommand(const string & command)
{
	if (ProcessCommandVariants(command, "seas", showSeas))								return true;
	if (ProcessCommandVariants(command, "lakes", showLakes))							return true;
	if (ProcessCommandVariants(command, "unbuildable", showUnbuildable))				return true;
	if (ProcessCommandVariants(command, "gh", showGroundHeight))						return true;
	if (ProcessCommandVariants(command, "minerals", showMinerals))						return true;
	if (ProcessCommandVariants(command, "geysers", showGeysers))						return true;
	if (ProcessCommandVariants(command, "static buildings", showStaticBuildings))		return true;
	if (ProcessCommandVariants(command, "bases", showBases))							return true;
	if (ProcessCommandVariants(command, "assigned ressources", showAssignedRessources))	return true;
	if (ProcessCommandVariants(command, "frontier", showFrontier))						return true;
	if (ProcessCommandVariants(command, "cp", showCP))									return true;
	
	if (ProcessCommandVariants(command, "all", showSeas))
	if (ProcessCommandVariants(command, "all", showLakes))
	if (ProcessCommandVariants(command, "all", showUnbuildable))
	if (ProcessCommandVariants(command, "all", showGroundHeight))
	if (ProcessCommandVariants(command, "all", showMinerals))
	if (ProcessCommandVariants(command, "all", showGeysers))
	if (ProcessCommandVariants(command, "all", showStaticBuildings))
	if (ProcessCommandVariants(command, "all", showBases))
	if (ProcessCommandVariants(command, "all", showAssignedRessources))
	if (ProcessCommandVariants(command, "all", showFrontier))
	if (ProcessCommandVariants(command, "all", showCP))
		return true;

	return false;
}

}} // namespace BWEM::utils



