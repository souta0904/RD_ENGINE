#include "StringUtility.h"
#include <Windows.h>

namespace rd::StringUtility
{
	std::wstring ToWide(std::string_view utf8)
	{
		if (utf8.empty()) return {};
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), int(utf8.size()), nullptr, 0);
		std::wstring result(size_t(sizeNeeded), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, utf8.data(), int(utf8.size()), result.data(), sizeNeeded);
		return result;
	}

	std::string ToUtf8(std::wstring_view wide)
	{
		if (wide.empty()) return {};
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wide.data(), int(wide.size()), nullptr, 0, nullptr, nullptr);
		std::string result(size_t(sizeNeeded), '\0');
		WideCharToMultiByte(CP_UTF8, 0, wide.data(), int(wide.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}
}
