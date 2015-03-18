#include "HLStatistics.h"

using namespace UAlbertaBot;

HLStatistics::~HLStatistics()
{
}
HLStatistics::HLStatistics() {
	// TODO Auto-generated constructor stub
	clear();
}

std::string HLStatistics::toString() const{
	char buff[80];
	sprintf_s(buff, 80, "%6.3f [%10d %.2e %5.3f %5.1f]",
		getRunningTimeMilliSecs() / 1000.0,
		getNodeCount(),
		getNodesSec(),
		//getTTqueries(),
		//getTTfound(),
		//getTTusefull(),
		//getTTcutoff(),
		getAvgBf(),
		getAvgFwd());
	return buff;
}
std::string HLStatistics::header() const {
	return "Time       nodes    nodes/sec   AvgBF	AvgFwd";
}
