#pragma once

namespace AKBot
{
	class Logger
	{
	public:
		virtual ~Logger() noexcept = default;
		/// <summary>Prints text to the logger sink.</summary> 
		/// 
		/// <param name="format">
		///   Text formatting. See std::printf for more information. Refrain from passing non-constant
		///   strings directly in this parameter.
		/// </param>
		/// <param name="...">
		///   The arguments that will be formatted using the given text formatting.
		/// </param>
		///
		/// @see Text::Enum, std::printf
		virtual void log(const char *format, ...) const = 0;
	};
}