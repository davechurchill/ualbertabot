/* 
 +----------------------------------------------------------------------+
 | UAlbertaBot                                                          |
 | Author: David Churchill <dave.churchill@gmail.com>                   |
 | https://github.com/davechurchill/ualbertabot                         |
 +----------------------------------------------------------------------+
*/

// BWAPI Libraries
#include <BWAPI.h>
#include <BWAPI/Client.h>

// C++ Libraries
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>

// 3rd Party Libraries
#include "rapidjson\document.h"

// UAlbertaBot Libraries
#include "UAlbertaBotModule.h"
#include "JSONTools.h"
#include "ParseUtils.h"
#include "UnitUtil.h"

using namespace UAlbertaBot;

void UAlbertaBot_BWAPIReconnect() 
{
    while(!BWAPI::BWAPIClient.connect())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 }); 
    }
}

void UAlbertaBot_PlayGame()
{
    // The UAlbertaBot module which will handle all the game logic
    // All Starcraft logic is in this object, when it destructs it's all cleaned up for the next game
    UAlbertaBotModule m;

    // The main game loop, which continues while we are connected to BWAPI and in a game
	while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame()) 
    {
        // Handle each of the events that happened on this frame of the game
		for (const BWAPI::Event & e : BWAPI::Broodwar->getEvents()) 
        {
			switch (e.getType()) 
            {
                case BWAPI::EventType::MatchStart:      { m.onStart(); break; }
                case BWAPI::EventType::MatchEnd:        { m.onEnd(e.isWinner()); break; }
			    case BWAPI::EventType::MatchFrame:      { m.onFrame(); break; }
			    case BWAPI::EventType::UnitShow:        { m.onUnitShow(e.getUnit()); break; }
			    case BWAPI::EventType::UnitHide:        { m.onUnitHide(e.getUnit()); break; }
			    case BWAPI::EventType::UnitCreate:      { m.onUnitCreate(e.getUnit()); break; }
			    case BWAPI::EventType::UnitMorph:       { m.onUnitMorph(e.getUnit()); break; }
			    case BWAPI::EventType::UnitDestroy:     { m.onUnitDestroy(e.getUnit()); break; }
			    case BWAPI::EventType::UnitRenegade:    { m.onUnitRenegade(e.getUnit()); break; }
			    case BWAPI::EventType::UnitComplete:    { m.onUnitComplete(e.getUnit()); break; }
			    case BWAPI::EventType::SendText:        { BWAPI::Broodwar->sendText("%s", e.getText().c_str()); break; }
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

int main(int argc, const char * argv[]) 
{
    bool exitIfStarcraftShutdown = true;

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
                std::cout << "Playing game " << gameCount++ << "\n";

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
