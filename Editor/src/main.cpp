#include <Windows.h>
#include "core/Log.h"
#include "Engine.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	rd::Log::Initialize("logs/Log.txt");

	rd::EngineConfig config;
	config.title = "Engine Test";

	rd::Engine engine;
	int result = engine.Run(config);

	rd::Log::Shutdown();
	return result;
}
