#include "UnitInfoManagerDebug.h"
#include "..\DebugTools.h"

namespace AKBot
{
	using UAlbertaBot::UnitInfo;
	using UAlbertaBot::DebugTools::DrawUnitHPBar;

	UnitInfoManagerDebug::UnitInfoManagerDebug(
		shared_ptr<OpponentView> opponentView,
		shared_ptr<UnitInfoManager> unitInfo,
		shared_ptr<AKBot::UnitInformation> unitInformation,
		const BotDebugConfiguration& debugConfiguration)
		: _opponentView(opponentView)
		, _unitInfo(unitInfo)
		, _unitInformation(unitInformation)
		, _debugConfiguration(debugConfiguration)
	{
	}

	void UnitInfoManagerDebug::draw(ScreenCanvas& canvas, int currentFrame)
	{
		drawExtendedInterface(canvas);
		drawUnitInformation(canvas, 425, 30);
	}

	void UnitInfoManagerDebug::drawExtendedInterface(AKBot::ScreenCanvas& canvas) const
	{
		if (!_debugConfiguration.DrawUnitHealthBars)
		{
			return;
		}

		// draw enemy units
		auto enemy = _opponentView->defaultEnemy();
		if (enemy == nullptr)
		{
			return;
		}

		for (const auto & kv : _unitInfo->getUnitData(enemy).getUnitInfoMap())
		{
			const UnitInfo & ui(kv.second);

			if (!ui.type.isResourceContainer())
			{
				DrawUnitHPBar(canvas, ui.type, ui.lastPosition, ui.lastHealth, ui.lastShields);
			}
		}

		// draw neutral units and our units
		for (auto & unit : _unitInformation->getAllUnits())
		{
			if (unit->getPlayer() == enemy)
			{
				continue;
			}

			DrawUnitHPBar(canvas, unit->getType(), unit->getPosition(), unit->getHitPoints(), unit->getShields());
		}
	}

	void UnitInfoManagerDebug::drawUnitInformation(AKBot::ScreenCanvas& canvas, int x, int y) const
	{
		if (!_debugConfiguration.DrawEnemyUnitInfo)
		{
			return;
		}

		auto enemy = _opponentView->defaultEnemy();
		if (enemy == __nullptr)
		{
			return;
		}

		std::string prefix = "\x04";

		auto selfUnitData = _unitInfo->getUnitData(_opponentView->self());
		auto enemyUnitData = _unitInfo->getUnitData(enemy);
		canvas.drawTextScreen(x, y - 10, "\x03 Self Loss:\x04 Minerals: \x1f%d \x04Gas: \x07%d", selfUnitData.getMineralsLost(), selfUnitData.getGasLost());
		canvas.drawTextScreen(x, y, "\x03 Enemy Loss:\x04 Minerals: \x1f%d \x04Gas: \x07%d", enemyUnitData.getMineralsLost(), enemyUnitData.getGasLost());
		canvas.drawTextScreen(x, y + 10, "\x04 Enemy: %s", enemy->getName().c_str());
		canvas.drawTextScreen(x, y + 20, "\x04 UNIT NAME");
		canvas.drawTextScreen(x + 140, y + 20, "\x04#");
		canvas.drawTextScreen(x + 160, y + 20, "\x04X");

		int yspace = 0;

		// for each unit in the queue
		for (BWAPI::UnitType t : BWAPI::UnitTypes::allUnitTypes())
		{
			int numUnits = enemyUnitData.getNumUnits(t);
			int numDeadUnits = enemyUnitData.getNumDeadUnits(t);

			// if there exist units in the vector
			if (numUnits > 0)
			{
				if (t.isDetector()) { prefix = "\x10"; }
				else if (t.canAttack()) { prefix = "\x08"; }
				else if (t.isBuilding()) { prefix = "\x03"; }
				else { prefix = "\x04"; }

				canvas.drawTextScreen(x, y + 40 + ((yspace) * 10), " %s%s", prefix.c_str(), t.getName().c_str());
				canvas.drawTextScreen(x + 140, y + 40 + ((yspace) * 10), "%s%d", prefix.c_str(), numUnits);
				canvas.drawTextScreen(x + 160, y + 40 + ((yspace++) * 10), "%s%d", prefix.c_str(), numDeadUnits);
			}
		}
	}
}