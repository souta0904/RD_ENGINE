#include "Engine.h"
#include "core/Log.h"
#include "platform/Window.h"

namespace rd
{
	int Engine::Run(const EngineConfig& config)
	{
		Log::Info("Engine: Started");

		Window window;
		if (!window.Initialize(config.clientWidth, config.clientHeight, config.title))
		{
			return -1;
		}

		while (!window.ProcessMessage())
		{
		}

		window.Terminate();
		Log::Info("Engine: Finished");
		return 0;
	}
}
