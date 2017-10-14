#include "BWAPIPrintLogger.h"
#include "BWAPI.h"

namespace AKBot
{
	void BWAPIPrintLogger::log(const char *format, ...) const
	{
		va_list args;
		va_start(args, format);
		BWAPI::Broodwar->vPrintf(format, args);
		va_end(args);
	}
}