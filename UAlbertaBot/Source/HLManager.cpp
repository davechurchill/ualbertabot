#include "HLManager.h"

using namespace UAlbertaBot;

HLManager::HLManager()
{
}


HLManager::~HLManager()
{
}

HLManager &	HLManager::Instance()
{
	static HLManager instance;
	return instance;
}

void HLManager::update()
{
	//scoutManager.update(HLSearch::Instance().getScouts());
	StrategyManager::Instance().setCurrentStrategy(HLSearch::Instance().getStrategy());
}