#pragma once
#include "Api.h"
#include "EngineConfig.h"

namespace rd
{
	/// <summary>
	/// エンジン本体
	/// </summary>
	class RD_API Engine
	{
	public:
		int Run(const EngineConfig& config);
	};
}
