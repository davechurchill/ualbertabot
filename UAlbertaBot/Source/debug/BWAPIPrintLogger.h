#pragma once
#include "Logger.h"

namespace AKBot
{
	class BWAPIPrintLogger : public Logger
	{
	public:
		~BWAPIPrintLogger() override = default;
		virtual void log(const char *format, ...) const override;
	};
}