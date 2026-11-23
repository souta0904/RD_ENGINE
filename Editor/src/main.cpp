#include "core/Log.h"
#include "platform/Window.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	rd::Log::Initialize("logs/Log.txt");
	rd::Log::Debug("Editor: Started");

	rd::Window window;
	if (window.Initialize(1920, 1080, "RD ENGINE"))
	{
		while (!window.ProcessMessage())
		{
		}
		window.Terminate();
	}

	rd::Log::Shutdown();
	return 0;
}
