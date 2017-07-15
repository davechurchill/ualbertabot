#include "GameCommanderDebug.h"
#include "Global.h"
#include "CombatCommanderDebug.h"
#include "ScoutManagerDebug.h"
#include "ProductionManagerDebug.h"

namespace AKBot
{
	using UAlbertaBot::Global::getEnemy;

	GameCommanderDebug::GameCommanderDebug(const GameCommander & gameCommander)
		: _gameCommander(gameCommander)
	{
	}

	void GameCommanderDebug::draw(ScreenCanvas & canvas) const
	{
		if (Config::Debug::DrawScoutInfo)
		{
			ScoutManagerDebug scoutDebug(_gameCommander.getScoutManager());
			scoutDebug.drawScoutInformation(canvas, 200, 320);
		}

		ProductionManagerDebug productionDebug(_gameCommander.getProductionManager());
		productionDebug.drawProductionInformation(canvas, 30, 50);
		CombatCommanderDebug combatDebug(_gameCommander.getCombatCommander());
		combatDebug.draw(canvas);

		drawGameInformation(canvas, 4, 1);

		// draw position of mouse cursor
		if (Config::Debug::DrawMouseCursorInfo)
		{
			int mouseX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
			int mouseY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;
			canvas.drawTextMap(mouseX + 20, mouseY, " %d %d", mouseX, mouseY);
		}
	}

	void GameCommanderDebug::drawGameInformation(ScreenCanvas& canvas, int x, int y) const
	{
		auto enemy = getEnemy();
		auto self = _gameCommander.getOpponentView().self();
		canvas.drawTextScreen(x, y, "\x04Players:");
		if (enemy != nullptr)
		{
			canvas.drawTextScreen(x + 50, y, "%c%s \x04vs. %c%s", self->getTextColor(), self->getName().c_str(),
				enemy->getTextColor(), enemy->getName().c_str());
		}

		y += 12;

		canvas.drawTextScreen(x, y, "\x04Strategy:");
		canvas.drawTextScreen(x + 50, y, "\x03%s %s", Config::Strategy::StrategyName.c_str(), Config::Strategy::FoundEnemySpecificStrategy ? "(enemy specific)" : "");
		canvas.setTextSize();
		y += 12;

		canvas.drawTextScreen(x, y, "\x04Map:");
		canvas.drawTextScreen(x + 50, y, "\x03%s", BWAPI::Broodwar->mapFileName().c_str());
		canvas.setTextSize();
		y += 12;

		canvas.drawTextScreen(x, y, "\x04Time:");
		canvas.drawTextScreen(x + 50, y, "\x04%d %4dm %3ds", BWAPI::Broodwar->getFrameCount(), (int)(BWAPI::Broodwar->getFrameCount() / (23.8 * 60)), (int)((int)(BWAPI::Broodwar->getFrameCount() / 23.8) % 60));
	}

}