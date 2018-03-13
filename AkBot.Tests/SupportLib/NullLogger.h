#pragma once
#include "Logger.h"

namespace AkBotTests
{
	class NullLogger : public AKBot::Logger
	{
	public:
		virtual void log(const char *format, ...) const override;
	};
}

