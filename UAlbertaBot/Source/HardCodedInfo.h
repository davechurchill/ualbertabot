/*#pragma once

#include "Common.h"
#include "BWTA.h"

#include <map>

class HardCodedInfo {

	std::ofstream logStream;
	std::string logFile;

	HardCodedInfo();
	static HardCodedInfo *				instance;

	void	setChokeDefendLocations();

	
	std::map<std::string, std::vector<BWAPI::Position>>		mainChokes;
	std::map<std::string, std::vector<BWAPI::Position>>		naturalChokes;

public:

	enum ChokeType {MAIN_CHOKE, NATURAL_CHOKE, CHOKE_TYPES};

	static HardCodedInfo *	Instance;

	void drawChokePoints();

	BWAPI::Position getChokepoint(ChokeType type, BWAPI::Player * player);
};
*/