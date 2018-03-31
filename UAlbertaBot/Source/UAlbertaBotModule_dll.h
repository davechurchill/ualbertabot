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

	void	onStart() override;
	void	onFrame() override;
	void	onEnd(bool isWinner) override;
	void	onUnitDestroy(BWAPI::Unit unit) override;
	void	onUnitMorph(BWAPI::Unit unit) override;
	void	onSendText(std::string text) override;
	void	onUnitCreate(BWAPI::Unit unit) override;
	void	onUnitComplete(BWAPI::Unit unit) override;
	void	onUnitShow(BWAPI::Unit unit) override;
	void	onUnitHide(BWAPI::Unit unit) override;
	void	onUnitRenegade(BWAPI::Unit unit) override;

	void onNukeDetect(BWAPI::Position target) override;
	void onUnitEvade(BWAPI::Unit unit) override;
};

}