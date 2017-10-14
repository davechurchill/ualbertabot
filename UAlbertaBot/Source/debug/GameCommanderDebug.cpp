#include "GameCommanderDebug.h"
#include "CombatCommanderDebug.h"
#include "ScoutManagerDebug.h"
#include "ProductionManagerDebug.h"

namespace AKBot
{
	GameCommanderDebug::GameCommanderDebug(
		shared_ptr<GameCommander> gameCommander,
		shared_ptr<AKBot::Logger> logger,
		const BotDebugConfiguration& debugConfiguration,
		const BotStrategyConfiguration& strategyConfiguration)
		: _gameCommander(gameCommander)
		, _logger(logger)
		, _debugConfiguration(debugConfiguration)
		, _strategyConfiguration(strategyConfiguration)
	{
	}

	void GameCommanderDebug::draw(ScreenCanvas & canvas)
	{
		if (_debugConfiguration.DrawScoutInfo)
		{
			ScoutManagerDebug scoutDebug(_gameCommander->getScoutManager());
			scoutDebug.drawScoutInformation(canvas, 200, 320);
		}

		ProductionManagerDebug productionDebug(_gameCommander->getProductionManager(), _logger, _debugConfiguration);
		productionDebug.drawProductionInformation(canvas, 30, 50);
		CombatCommanderDebug combatDebug(_gameCommander->getCombatCommander(), _debugConfiguration);
		combatDebug.draw(canvas);

		drawGameInformation(canvas, 4, 1);

		// draw position of mouse cursor
		if (_debugConfiguration.DrawMouseCursorInfo)
		{
			int mouseX = BWAPI::Broodwar->getMousePosition().x + BWAPI::Broodwar->getScreenPosition().x;
			int mouseY = BWAPI::Broodwar->getMousePosition().y + BWAPI::Broodwar->getScreenPosition().y;
			canvas.drawTextMap(mouseX + 20, mouseY, " %d %d", mouseX, mouseY);
		}
	}

	void GameCommanderDebug::drawGameInformation(ScreenCanvas& canvas, int x, int y) const
	{
		auto enemy = _gameCommander->getOpponentView()->defaultEnemy();
		auto self = _gameCommander->getOpponentView()->self();
		canvas.drawTextScreen(x, y, "\x04Players:");
		if (enemy != nullptr)
		{
			canvas.drawTextScreen(x + 50, y, "%c%s \x04vs. %c%s", self->getTextColor(), self->getName().c_str(),
				enemy->getTextColor(), enemy->getName().c_str());
		}

		y += 12;

		canvas.drawTextScreen(x, y, "\x04Strategy:");
		canvas.drawTextScreen(x + 50, y, "\x03%s %s", _strategyConfiguration.StrategyName.c_str(), _strategyConfiguration.FoundEnemySpecificStrategy ? "(enemy specific)" : "");
		canvas.setTextSize();
		y += 12;

		canvas.drawTextScreen(x, y, "\x04Map:");
		canvas.drawTextScreen(x + 50, y, "\x03%s", BWAPI::Broodwar->mapFileName().c_str());
		canvas.setTextSize();
		y += 12;

		auto frameCount = BWAPI::Broodwar->getFrameCount();
		canvas.drawTextScreen(x, y, "\x04Time:");
		canvas.drawTextScreen(x + 50, y, "\x04%d %4dm %3ds", frameCount, (int)(frameCount / (23.8 * 60)), (int)((int)(frameCount / 23.8) % 60));
	}

}