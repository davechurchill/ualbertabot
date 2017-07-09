#pragma once

#include <Common.h>
#include <BWAPI/Unit.h>
#include "ScreenCanvas.h"

namespace UAlbertaBot
{
namespace Micro
{
	const BWAPI::Color MoveColor = BWAPI::Colors::White;
	const BWAPI::Color AttackMoveColor = BWAPI::Colors::Orange;
	const BWAPI::Color AttackUnitColor = BWAPI::Colors::Red;
	const BWAPI::Color RightClickColor = BWAPI::Colors::Cyan;
	const BWAPI::Color RepairColor = BWAPI::Colors::Cyan;

	void SetOnAttackUnit(std::function<void(const BWAPI::Unit& attacker, const BWAPI::Unit& target)> handler);
	void SetOnAttackMove(std::function<void(const BWAPI::Unit& attacker, const BWAPI::Position & targetPosition)> handler);
	void SetOnMove(std::function<void(const BWAPI::Unit& attacker, const BWAPI::Position & targetPosition)> handler);
	void SetOnRepair(std::function<void(const BWAPI::Unit& unit, const BWAPI::Unit & target)> handler);
	void SetOnRightClick(std::function<void(const BWAPI::Unit& unit, const BWAPI::Unit & target)> handler);

	void SmartAttackUnit(BWAPI::Unit attacker, BWAPI::Unit target);
    void SmartAttackMove(BWAPI::Unit attacker, const BWAPI::Position & targetPosition);
    void SmartMove(BWAPI::Unit attacker, const BWAPI::Position & targetPosition);
    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);
    void SmartLaySpiderMine(BWAPI::Unit unit, BWAPI::Position pos);
    void SmartRepair(BWAPI::Unit unit, BWAPI::Unit target);
    void SmartKiteTarget(BWAPI::Unit rangedUnit, BWAPI::Unit target);
    void MutaDanceTarget(BWAPI::Unit muta, BWAPI::Unit target);
    BWAPI::Position GetKiteVector(BWAPI::Unit unit, BWAPI::Unit target);

    void Rotate(double &x, double &y, double angle);
    void Normalize(double &x, double &y);

    void drawAPM(AKBot::ScreenCanvas& canvas, int x, int y);
	void drawAction(
		AKBot::ScreenCanvas& canvas,
		const BWAPI::Position& attackerPostion,
		const BWAPI::Position & targetPosition,
		const BWAPI::Color color);
};
}