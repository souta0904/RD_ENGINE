#include "GameLauncher.h"
#include "Engine.h"
#include "core/Log.h"
#include "runtime/GameModule.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

namespace rd
{
	namespace
	{
		constexpr const char* kGameName = "Sandbox";
	}

	int RunEditorLauncher()
	{
		Log::Initialize("logs/Log.txt");

		// dllを読み込む
		if (!GameModule::Load(kGameName))
		{
			MessageBoxW(nullptr, L"Failed to load the game DLL. See logs/Log.txt", L"RD ENGINE", MB_OK | MB_ICONERROR);
			Log::Shutdown();
			return -1;
		}

		// ゲーム側に設定を書いてもらう
		EngineConfig config;
		GameModule::ConfigureFunc configure = GameModule::GetConfigure();
		configure(config);

		Engine engine;
		int result = engine.Run(config);

		Log::Shutdown();
		return result;
	}
}
