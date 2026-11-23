#pragma once
#include "Api.h"
#include <format>
#include <string>

namespace rd::Log
{
	enum class Level
	{
		Debug,
		Info,
		Warning,
		Error,
	};

	RD_API void Initialize(const std::string& path);
	RD_API void Shutdown();

	RD_API void Write(Level level, const std::string& text);

	RD_API void SetMinLevel(Level minLevel);

#define LOG_FUNCTION(Name, Level)                                    \
	template <class... Args>                                         \
	void Name(std::format_string<Args...> fmt, Args&&... args)       \
	{                                                                \
		Write(Level, std::format(fmt, std::forward<Args>(args)...)); \
	}

	LOG_FUNCTION(Debug, Level::Debug)
	LOG_FUNCTION(Info, Level::Info)
	LOG_FUNCTION(Warning, Level::Warning)
	LOG_FUNCTION(Error, Level::Error)
#undef LOG_FUNCTION
}
