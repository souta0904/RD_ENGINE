#include "core/Log.h"
#include "core/utility/StringUtility.h"
#include <filesystem>
#include <fstream>
#include <mutex>
#include <Windows.h>

namespace rd::Log
{
	namespace
	{
		std::mutex sMutex;// sFile用
		std::ofstream sFile;

		Level sMinLevel = Level::Debug;

		const char* ToString(Level level)
		{
			switch (level)
			{
			case Level::Debug: return "Debug";
			case Level::Info: return "Info";
			case Level::Warning: return "Warning";
			case Level::Error: return "Error";
			}

			return "Unknown";
		}
	}

	void Initialize(const std::string& path)
	{
		std::lock_guard lock(sMutex);
		std::filesystem::path p(StringUtility::ToWide(path));
		if (p.has_parent_path())
		{
			std::filesystem::create_directories(p.parent_path());
		}

		sFile.open(p, std::ios::out | std::ios::trunc);
	}

	void Shutdown()
	{
		std::lock_guard lock(sMutex);
		sFile.close();
	}

	void Write(Level level, const std::string& text)
	{
		std::lock_guard lock(sMutex);
		if (level < sMinLevel) return;

		SYSTEMTIME time;
		GetLocalTime(&time);
		std::string message = std::format("[{:02}:{:02}:{:02}][{}] {}\n", time.wHour, time.wMinute, time.wSecond, ToString(level), text);

		// 出力ウィンドウ
		OutputDebugStringW(StringUtility::ToWide(message).c_str());

		// ファイル
		if (sFile.is_open())
		{
			sFile << message;
			sFile.flush();
		}
	}

	void SetMinLevel(Level minLevel)
	{
		std::lock_guard lock(sMutex);
		sMinLevel = minLevel;
	}
}
