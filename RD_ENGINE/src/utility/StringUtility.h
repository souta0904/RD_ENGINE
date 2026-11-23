#pragma once
#include "Api.h"
#include <string>
#include <string_view>

namespace rd::StringUtility
{
	RD_API std::wstring ToWide(std::string_view utf8);
	RD_API std::string ToUtf8(std::wstring_view wide);
}
