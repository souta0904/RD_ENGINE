#include "GameLauncher.h"
#include "core/Log.h"

namespace
{
	void ConfigureGame(rd::EngineConfig& config)
	{
		rd::Log::Info("Sandbox: ConfigureGame()");
		config.title = "Sandbox";
		config.clientWidth = 1920;
		config.clientHeight = 1080;
	}
}

RD_GAME_MAIN(ConfigureGame)
