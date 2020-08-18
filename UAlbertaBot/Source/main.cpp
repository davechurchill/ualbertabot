#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "UAlbertaBotModule.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "Global.h"
#include "../../SparCraft/source/SparCraft.h"
#include "../../BOSS/source/BOSS.h"

using namespace BWAPI;
using namespace UAlbertaBot;

void UAlbertaBot_BWAPIReconnect()
{
	while (!BWAPIClient.connect())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
	}
}

void UAlbertaBot_PlayGame()
{
    PROFILE_FUNCTION();

	UAlbertaBotModule bot;

	// The main game loop, which continues while we are connected to BWAPI and in a game
	while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame())
	{
		// Handle each of the events that happened on this frame of the game
		for (const BWAPI::Event & e : BWAPI::Broodwar->getEvents())
		{
			switch (e.getType())
			{
				case BWAPI::EventType::MatchStart:      { bot.onStart();					  break; }
				case BWAPI::EventType::MatchFrame:      { bot.onFrame();                      break; }
				case BWAPI::EventType::MatchEnd:        { bot.onEnd(e.isWinner());            break; }
				case BWAPI::EventType::UnitShow:        { bot.onUnitShow(e.getUnit());        break; }
				case BWAPI::EventType::UnitHide:        { bot.onUnitHide(e.getUnit());        break; }
				case BWAPI::EventType::UnitCreate:      { bot.onUnitCreate(e.getUnit());      break; }
				case BWAPI::EventType::UnitMorph:       { bot.onUnitMorph(e.getUnit());       break; }
				case BWAPI::EventType::UnitDestroy:     { bot.onUnitDestroy(e.getUnit());     break; }
				case BWAPI::EventType::UnitRenegade:    { bot.onUnitRenegade(e.getUnit());    break; }
				case BWAPI::EventType::UnitComplete:    { bot.onUnitComplete(e.getUnit());    break; }
				case BWAPI::EventType::SendText:        { bot.onSendText(e.getText());        break; }
			}
		}
        
		BWAPI::BWAPIClient.update();
		if (!BWAPI::BWAPIClient.isConnected())
		{
			std::cout << "Disconnected\n";
			break;
		}
	}

	std::cout << "Game Over\n";
}


int main(int argc, char * argv[])
{
    PROFILE_FUNCTION();

	bool exitIfStarcraftShutdown = true;
    
    std::cout << "UAlbertaBot - David Churchill\n";
    std::cout << "https://github.com/davechurchill/ualbertabot\n\n";
    std::cout << "Compiled on " << __DATE__ << " @ " << __TIME__ << "\n\n";

    // Initialize SparCraft, the combat simulation package
    SparCraft::init();

    // Initialize BOSS, the Build Order Search System
    BOSS::init();

	size_t gameCount = 0;
	while (true)
	{
		// if we are not currently connected to BWAPI, try to reconnect
		if (!BWAPI::BWAPIClient.isConnected())
		{
			UAlbertaBot_BWAPIReconnect();
		}

		// if we have connected to BWAPI
		while (BWAPI::BWAPIClient.isConnected())
		{
			// wait for the game to start until the game starts
			std::cout << "Waiting for game start\n";
			while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame())
			{
				BWAPI::BWAPIClient.update();
			}

			// Check to see if Starcraft shut down somehow
			if (BWAPI::BroodwarPtr == nullptr)
			{
				break;
			}

			// If we are successfully in a game, call the module to play the game
			if (BWAPI::Broodwar->isInGame())
			{
				std::cout << "Playing game " << gameCount++ << " on map " << BWAPI::Broodwar->mapFileName() << "\n";

				UAlbertaBot_PlayGame();
			}
		}

		if (exitIfStarcraftShutdown && !BWAPI::BWAPIClient.isConnected())
		{
			return 0;
		}
	}

	return 0;
}