#pragma once

#include <BWAPI.h>
#include "GameCommander.h"
#include <iostream>
#include <fstream>
#include "Logger.h"
#include "MapTools.h"
#include "HardCodedInfo.h"
#include "EnhancedInterface.hpp"
#include "UnitCommandManager.h"
#include "SparCraftManager.h"

#include "Options.h"

namespace UAlbertaBot
{

class UAlbertaBotModule : public BWAPI::AIModule
{
	GameCommander			gameCommander;
	EnhancedInterface		eui;
    SparCraftManager        sparcraftManager;

public:
			
	UAlbertaBotModule();
	~UAlbertaBotModule();

	void	onStart();
	void	onFrame();
	void	onEnd(bool isWinner);
	void	onUnitDestroy(BWAPI::UnitInterface* unit);
	void	onUnitMorph(BWAPI::UnitInterface* unit);
	void	onSendText(std::string text);
	void	onUnitCreate(BWAPI::UnitInterface* unit);
	void	onUnitShow(BWAPI::UnitInterface* unit);
	void	onUnitHide(BWAPI::UnitInterface* unit);
	void	onUnitRenegade(BWAPI::UnitInterface* unit);
};

}