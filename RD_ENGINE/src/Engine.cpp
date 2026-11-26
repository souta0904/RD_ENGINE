#include "Engine.h"
#include "core/Assert.h"
#include "core/Log.h"
#include "platform/Window.h"
#include "rhi/device/GraphicsDevice.h"

namespace rd
{
	namespace
	{
		Engine* sInstance = nullptr;
	}

	Engine& Engine::Get()
	{
		RD_ASSERT(sInstance);
		return *sInstance;
	}

	Engine::Engine()
	{
		RD_ASSERT(!sInstance);
		sInstance = this;
	}

	Engine::~Engine()
	{
		sInstance = nullptr;
	}

	int Engine::Run(const EngineConfig& config)
	{
		mConfig = config;
		Log::Info("Engine: Started");

		if (!Initialize())
		{
			Shutdown();
			return -1;
		}

		while (mIsRunning && !mWindow->ProcessMessage())
		{
			MainLoop();
		}

		Shutdown();
		Log::Info("Engine: Finished");
		return mExitCode;
	}

	void Engine::Quit(int exitCode)
	{
		mIsRunning = false;
		mExitCode = exitCode;
	}

	bool Engine::Initialize()
	{
		mWindow = std::make_unique<Window>();
		if (!mWindow->Initialize(mConfig.clientWidth, mConfig.clientHeight, mConfig.title)) return false;

		mGraphicsDevice = std::make_unique<GraphicsDevice>();
		if (!mGraphicsDevice->Initialize(*mWindow)) return false;

		return true;
	}

	void Engine::Shutdown()
	{
		if (mGraphicsDevice)
		{
			mGraphicsDevice->Shutdown();
			mGraphicsDevice.reset();
		}

		if (mWindow)
		{
			mWindow->Shutdown();
			mWindow.reset();
		}
	}

	void Engine::MainLoop()
	{
		if (mWindow->ConsumeResized())
		{
			if (!mGraphicsDevice->Resize(mWindow->GetClientWidth(), mWindow->GetClientHeight()))
			{
				Quit(-1);
				return;
			}
		}

		mGraphicsDevice->BeginFrame();
		mGraphicsDevice->EndFrame();
	}
}
