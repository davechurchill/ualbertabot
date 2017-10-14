#pragma once

#include <BWAPI/Player.h>
#include <memory>
#include "BotModule.h"
#include "AutoObserver.h"
#include "ArenaPlayer.h"
#include "BWAPIScreenCanvas.h"
#include "BotConfiguration.h"

namespace UAlbertaBot
{

class UAlbertaBot_Arena : public BotModule
{
	const BotConfiguration&			_configuration;
    std::shared_ptr<ArenaPlayer>    _arenaPlayer;
    std::string                     _startDate;
    std::string                     _resultString;
    std::vector<size_t>             _results;       // 0 = wins, 1 = losses, 2 = draws
    size_t                          _battles;
    bool                            _prevIsBattle;
	AKBot::BWAPIScreenCanvas		_canvas;
    
    void drawUnitHPBars(AKBot::ScreenCanvas& canvas) const;
    size_t numUnits(BWAPI::Player player) const;
    bool isBattle();
    int winner();

public:

    UAlbertaBot_Arena(const BotConfiguration& configuration);
	UAlbertaBot_Arena(const UAlbertaBot_Arena&) = delete;

    void    printInfo();

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