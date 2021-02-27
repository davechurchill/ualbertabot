#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "StarterBot.h"
#include <iostream>
#include <thread>
#include <chrono>

void PlayGame();

int main(int argc, char * argv[])
{
    size_t gameCount = 0;

    // if we are not currently connected to BWAPI, try to reconnect
    while (!BWAPI::BWAPIClient.connect())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
    }

    // if we have connected to BWAPI
    while (BWAPI::BWAPIClient.isConnected())
    {
        // the starcraft exe has connected but we need to wait for the game to start
        std::cout << "Waiting for game start\n";
        while (BWAPI::BWAPIClient.isConnected() && !BWAPI::Broodwar->isInGame())
        {
            BWAPI::BWAPIClient.update();
        }

        // Check to see if Starcraft shut down somehow
        if (BWAPI::BroodwarPtr == nullptr) { break; }

        // If we are successfully in a game, call the module to play the game
        if (BWAPI::Broodwar->isInGame())
        {
            std::cout << "Playing game " << gameCount++ << " on map " << BWAPI::Broodwar->mapFileName() << "\n";

            PlayGame();
        }
    }



	return 0;
}

void PlayGame()
{
    StarterBot bot;

    // The main game loop, which continues while we are connected to BWAPI and in a game
    while (BWAPI::BWAPIClient.isConnected() && BWAPI::Broodwar->isInGame())
    {
        // Handle each of the events that happened on this frame of the game
        for (const BWAPI::Event& e : BWAPI::Broodwar->getEvents())
        {
            switch (e.getType())
            {
                case BWAPI::EventType::MatchStart:   { bot.onStart();                       break; }
                case BWAPI::EventType::MatchFrame:   { bot.onFrame();                       break; }
                case BWAPI::EventType::MatchEnd:     { bot.onEnd(e.isWinner());             break; }
                case BWAPI::EventType::UnitShow:     { bot.onUnitShow(e.getUnit());         break; }
                case BWAPI::EventType::UnitHide:     { bot.onUnitHide(e.getUnit());         break; }
                case BWAPI::EventType::UnitCreate:   { bot.onUnitCreate(e.getUnit());       break; }
                case BWAPI::EventType::UnitMorph:    { bot.onUnitMorph(e.getUnit());        break; }
                case BWAPI::EventType::UnitDestroy:  { bot.onUnitDestroy(e.getUnit());      break; }
                case BWAPI::EventType::UnitRenegade: { bot.onUnitRenegade(e.getUnit());     break; }
                case BWAPI::EventType::UnitComplete: { bot.onUnitComplete(e.getUnit());     break; }
                case BWAPI::EventType::SendText:     { bot.onSendText(e.getText());         break; }
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
