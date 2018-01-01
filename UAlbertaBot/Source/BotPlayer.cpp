#include "BotPlayer.h"
#include "BWAPI.h"
#include <BWAPI/Client.h>

using namespace AKBot;

void BotPlayer::PlayGame()
{
	// The main game loop, which continues while we are connected to BWAPI and in a game
	while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame())
	{
		// Handle each of the events that happened on this frame of the game
		for (const BWAPI::Event & e : BWAPI::Broodwar->getEvents())
		{
			switch (e.getType())
			{
			case BWAPI::EventType::MatchStart: { m->onStart();                      break; }
			case BWAPI::EventType::MatchEnd: { m->onEnd(e.isWinner());            break; }
			case BWAPI::EventType::MatchFrame: { m->onFrame();                      break; }
			case BWAPI::EventType::UnitShow: { m->onUnitShow(e.getUnit());        break; }
			case BWAPI::EventType::UnitHide: { m->onUnitHide(e.getUnit());        break; }
			case BWAPI::EventType::UnitCreate: { m->onUnitCreate(e.getUnit());      break; }
			case BWAPI::EventType::UnitMorph: { m->onUnitMorph(e.getUnit());       break; }
			case BWAPI::EventType::UnitDestroy: { m->onUnitDestroy(e.getUnit());     break; }
			case BWAPI::EventType::UnitRenegade: { m->onUnitRenegade(e.getUnit());    break; }
			case BWAPI::EventType::UnitComplete: { m->onUnitComplete(e.getUnit());    break; }
			case BWAPI::EventType::SendText: { m->onSendText(e.getText());        break; }
			}
		}

		BWAPI::BWAPIClient.update();
		if (!BWAPI::BWAPIClient.isConnected())
		{
			std::cout << "Disconnected\n";
			break;
		}
	}

	std::cout << "Game Over at frame " << BWAPI::Broodwar->getFrameCount() << std::endl;
}