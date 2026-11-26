#pragma once
#include "Api.h"
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace rd
{
	using Json = nlohmann::ordered_json;

	namespace JsonUtility
	{
		RD_API bool LoadFile(const std::string& path, Json& out);
		RD_API bool SaveFile(const std::string& path, const Json& json);

		RD_API bool GetBool(const Json& json, const char* name, bool& out);
		RD_API bool GetInt(const Json& json, const char* name, int32_t& out);
		RD_API bool GetUint(const Json& json, const char* name, uint32_t& out);
		RD_API bool GetFloat(const Json& json, const char* name, float& out);
		RD_API bool GetString(const Json& json, const char* name, std::string& out);
	}
}
