#pragma once
#include "Logger.h"

namespace AKBot
{
	class BWAPIPrintLogger : public Logger
	{
	public:
		virtual void log(const char *format, ...) const override;
	};
}