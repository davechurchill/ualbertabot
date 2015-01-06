#pragma once;

#include <Common.h>
#include "MicroManager.h"

namespace UAlbertaBot
{
class MicroManager;

class TransportManager : public MicroManager
{

public:

	TransportManager();
	~TransportManager() {}

	void executeMicro(const std::vector<BWAPI::UnitInterface *> & targets);
};
}