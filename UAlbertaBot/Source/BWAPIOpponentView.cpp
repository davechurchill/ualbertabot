#include "BWAPIOpponentView.h"
#include <BWAPI\Game.h>

BWAPI::Player AKBot::BWAPIOpponentView::self() const
{
	return BWAPI::Broodwar->self();
}

BWAPI::Playerset & AKBot::BWAPIOpponentView::enemies() const
{
	return BWAPI::Broodwar->enemies();
}
