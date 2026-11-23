#pragma once
#include <cstdint>
#include <string>

namespace rd
{
	struct EngineConfig
	{
		uint32_t clientWidth = 1920;
		uint32_t clientHeight = 1080;
		std::string title = "RD ENGINE";
	};
}
