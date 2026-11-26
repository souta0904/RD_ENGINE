#pragma once
#include "Api.h"
#include "EngineConfig.h"

namespace rd
{
	RD_API int RunEditorLauncher();
}

#define RD_GAME_MAIN(ConfigureFunction) \
extern "C" __declspec(dllexport) void RdConfigureGame(::rd::EngineConfig& config) \
{ \
	ConfigureFunction(config); \
}
