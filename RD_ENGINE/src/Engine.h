#pragma once
#include "Api.h"
#include "EngineConfig.h"
#include <memory>

namespace rd
{
	class GraphicsDevice;
	class Window;

	/// <summary>
	/// エンジン本体
	/// </summary>
	class RD_API Engine
	{
	public:
		static Engine& Get();

		Engine();
		~Engine();

		int Run(const EngineConfig& config);

		void Quit(int exitCode = 0);

		Window& GetWindow() { return *mWindow; }
		GraphicsDevice& GetGraphicsDevice() { return *mGraphicsDevice; }

	private:
		bool Initialize();
		void Shutdown();
		void MainLoop();

		EngineConfig mConfig;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<GraphicsDevice> mGraphicsDevice;

		bool mIsRunning = true;
		int mExitCode = 0;
	};
}
