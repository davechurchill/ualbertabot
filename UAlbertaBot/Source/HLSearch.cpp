#include "HLSearch.h"

using namespace UAlbertaBot;

HLSearch::HLSearch()
{
}


HLSearch::~HLSearch()
{
}

// get an instance of this
HLSearch & HLSearch::Instance()
{
	static HLSearch instance;
	return instance;
}

void HLSearch::search(double timeLimit)
{
}

int HLSearch::getStrategy()
{
	return 0;
}