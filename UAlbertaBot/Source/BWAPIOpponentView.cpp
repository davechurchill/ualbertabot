#include "BWAPIOpponentView.h"
#include <BWAPI\Game.h>

BWAPI::Player AKBot::BWAPIOpponentView::self()
{
	return BWAPI::Broodwar->self();
}

BWAPI::Playerset & AKBot::BWAPIOpponentView::enemies()
{
	return BWAPI::Broodwar->enemies();
}
