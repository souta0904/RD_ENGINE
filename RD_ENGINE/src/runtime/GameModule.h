#pragma once
#include "Api.h"
#include "EngineConfig.h"
#include <string>

namespace rd::GameModule
{
	// 設定関数の型
	using ConfigureFunc = void(*)(EngineConfig& config);

	RD_API bool Load(const std::string& name);

	RD_API ConfigureFunc GetConfigure();
}
