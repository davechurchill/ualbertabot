#pragma once

#include <BWAPI/AIModule.h>
#include <memory>
#include "BotModule.h"

namespace UAlbertaBot
{

class UAlbertaBotModule_dll : public BWAPI::AIModule
{
    std::shared_ptr<BotModule> _module;

public:

    UAlbertaBotModule_dll();

	void	onStart();
	void	onFrame();
	void	onEnd(bool isWinner);
	void	onUnitDestroy(BWAPI::Unit unit);
	void	onUnitMorph(BWAPI::Unit unit);
	void	onSendText(std::string text);
	void	onUnitCreate(BWAPI::Unit unit);
	void	onUnitComplete(BWAPI::Unit unit);
	void	onUnitShow(BWAPI::Unit unit);
	void	onUnitHide(BWAPI::Unit unit);
	void	onUnitRenegade(BWAPI::Unit unit);
};

}