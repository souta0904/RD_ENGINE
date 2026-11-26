#include "runtime/GameModule.h"
#include "core/Log.h"
#include "core/utility/StringUtility.h"
#include <Windows.h>

namespace rd::GameModule
{
	namespace
	{
		HMODULE sModule = nullptr;
		ConfigureFunc sConfigure = nullptr;
	}

	bool Load(const std::string& name)
	{
		// dllを読み込む
		std::wstring path = StringUtility::ToWide(name + ".dll");
		sModule = LoadLibraryW(path.c_str());
		if (!sModule)
		{
			DWORD error = GetLastError();
			Log::Error("GameModule: LoadLibraryW failed: {}.dll ({})", name, error);
			return false;
		}

		// 設定関数を名前で探す
		sConfigure = reinterpret_cast<ConfigureFunc>(GetProcAddress(sModule, "RdConfigureGame"));
		if (!sConfigure)
		{
			Log::Error("GameModule: RdConfigureGame not found in {}.dll", name);
			FreeLibrary(sModule);
			sModule = nullptr;
			return false;
		}

		Log::Info("GameModule: Loaded {}.dll", name);
		return true;
	}

	ConfigureFunc GetConfigure()
	{
		return sConfigure;
	}
}
