#include "GameDebug.h"

AKBot::GameDebug::GameDebug(vector<shared_ptr<DebugInfoProvider>> debugProviders)
	: _debugProviders(std::move(debugProviders))
{
}

void AKBot::GameDebug::draw(ScreenCanvas & canvas)
{
	for (auto& provider : _debugProviders)
	{
		provider->draw(canvas);
	}
}
