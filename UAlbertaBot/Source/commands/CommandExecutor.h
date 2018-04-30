#pragma once
#include <string>
#include <tuple>

namespace AKBot
{
	class CommandExecutor
	{
	public:
		/*
		 * Executes the command
		 */
		virtual void onCommand(const std::string& command) = 0;
		/*
		 * Checks whether command is supported
		 */
		virtual bool isSupported(const std::string& command) const {
			std::ignore = command;
			return true;
		}
	};
}