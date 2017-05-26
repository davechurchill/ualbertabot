#pragma once

#include <BWAPI.h>
#include "BotModule.h"
#include "../../SparCraft/source/SparCraft.h"
#include "AutoObserver.h"

namespace UAlbertaBot
{

class UAlbertaBot_Arena : public BotModule
{
    bool                            _isSparCraftInitialized;
    SparCraft::SparCraftException   _lastSparCraftException;
    std::string                     _startDate;
    std::string                     _resultString;

    std::vector<size_t>             _wins;
    std::vector<size_t>             _simWins;
    size_t                          _battles;

    int                             _prevNumUnits;

    bool                            _battleEnded;

    SparCraft::GameState    GetSparCraftState() const;
    SparCraft::Unit         GetSparCraftUnit(BWAPI::Unit unit) const;
    SparCraft::Move         GetSparCraftPlayerMove(const SparCraft::GameState & state, const size_t & playerID) const;
    size_t                  GetSparCraftPlayerID(BWAPI::Player player) const;
    void                    DoSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const;
    void                    DrawSparCraftState(const SparCraft::GameState & state, int x, int y) const;
    void                    DrawSparCraftMove(const SparCraft::GameState & state, const SparCraft::Move & move) const;
    void                    DrawUnitHPBars() const;

    int                     GetTimeSinceLastAttack(BWAPI::Unit unit) const;
    int                     GetTimeCanAttack(BWAPI::Unit unit) const;
    int                     GetTimeCanMove(BWAPI::Unit unit) const;

    void                    PlaySparCraftSimulation(const SparCraft::GameState & state);

public:

    UAlbertaBot_Arena();

    void    initializeSparCraft();
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