#include "SquadData.h"

using namespace UAlbertaBot;

SquadData::SquadData() 
{
	
}

void SquadData::update()
{
	updateAllSquads();
}

void SquadData::clearSquadData()
{
    // give back workers who were in squads
    for (Squad & squad : squads)
	{
        const std::vector<BWAPI::UnitInterface *> & units = squad.getUnits();

        for (size_t u(0); u<units.size(); ++u)
        {
            if (units[u]->getType().isWorker())
            {
                WorkerManager::Instance().finishedWithWorker(units[u]);
            }
        }
	}

	squads.clear();
}

void SquadData::addSquad(const Squad & squad)
{
	squads.push_back(squad);
}

void SquadData::updateAllSquads()
{
	for (Squad & squad : squads)
	{
		squad.update();
	}
}

void SquadData::drawSquadInformation(int x, int y) 
{
    if (!Config::Debug::DrawSquadInfo)
    {
        return;
    }

	BWAPI::Broodwar->drawTextScreen(x, y, "\x04Squads");
	BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04ORDER");
	BWAPI::Broodwar->drawTextScreen(x+100, y+20, "\x04SIZE");
	BWAPI::Broodwar->drawTextScreen(x+150, y+20, "\x04LOCATION");

	int yspace = 0;

	for (size_t i(0); i<squads.size(); i++) 
	{
		const std::vector<BWAPI::UnitInterface *> & units = squads[i].getUnits();
		const SquadOrder & order = squads[i].getSquadOrder();

		BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03%s", order.getStatus().c_str());
		BWAPI::Broodwar->drawTextScreen(x+100, y+40+((yspace)*10), "\x03%d", units.size());
		BWAPI::Broodwar->drawTextScreen(x+150, y+40+((yspace++)*10), "\x03(%d,%d)", order.position.x, order.position.y);

		BWAPI::Broodwar->drawCircleMap(order.position.x, order.position.y, 10, BWAPI::Colors::Green, true);
		BWAPI::Broodwar->drawCircleMap(order.position.x, order.position.y, 400, BWAPI::Colors::Green, false);
		BWAPI::Broodwar->drawCircleMap(order.position.x, order.position.y, 100, BWAPI::Colors::Green, false);
	}
}