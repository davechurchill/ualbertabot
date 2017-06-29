#pragma once

#include <BWAPI.h>
#include "BotModule.h"
#include "AutoObserver.h"
#include "ArenaPlayer.h"
#include <memory>

namespace UAlbertaBot
{

class UAlbertaBot_Arena : public BotModule
{
    std::shared_ptr<ArenaPlayer>    _arenaPlayer;
    std::string                     _startDate;
    std::string                     _resultString;
    std::vector<size_t>             _results;       // 0 = wins, 1 = losses, 2 = draws
    size_t                          _battles;
    bool                            _prevIsBattle;
    
    void drawUnitHPBars() const;
    size_t numUnits(BWAPI::Player player) const;
    bool isBattle();
    int winner();

public:

    UAlbertaBot_Arena();

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