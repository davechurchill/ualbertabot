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
    BOOST_FOREACH (Squad & squad, squads)
	{
        const UnitVector & units = squad.getUnits();

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
	BOOST_FOREACH (Squad & squad, squads)
	{
		squad.update();
	}
}

void SquadData::drawSquadInformation(int x, int y) 
{
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y, "\x04Squads");
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y+20, "\x04ORDER");
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+100, y+20, "\x04SIZE");
	if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+150, y+20, "\x04LOCATION");

	int yspace = 0;

	for (size_t i(0); i<squads.size(); i++) 
	{
		const UnitVector & units = squads[i].getUnits();
		const SquadOrder & order = squads[i].getSquadOrder();

		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x, y+40+((yspace)*10), "\x03%s", order.getStatus().c_str());
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+100, y+40+((yspace)*10), "\x03%d", units.size());
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawTextScreen(x+150, y+40+((yspace++)*10), "\x03(%d,%d)", order.position.x(), order.position.y());

		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(order.position.x(), order.position.y(), 10, BWAPI::Colors::Green, true);
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(order.position.x(), order.position.y(), 400, BWAPI::Colors::Green, false);
		if (Options::Debug::DRAW_UALBERTABOT_DEBUG) BWAPI::Broodwar->drawCircleMap(order.position.x(), order.position.y(), 100, BWAPI::Colors::Green, false);
	}
}