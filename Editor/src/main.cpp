#include "core/Log.h"
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	rd::Log::Initialize("logs/Log.txt");
	rd::Log::Debug("Editor: 起動しました");
	rd::Log::Shutdown();

	return 0;
}
