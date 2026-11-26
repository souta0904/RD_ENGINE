#include "core/utility/JsonUtility.h"
#include "core/Log.h"
#include "core/utility/StringUtility.h"
#include <filesystem>
#include <fstream>

namespace rd::JsonUtility
{
	namespace
	{
		const Json* Find(const Json& json, const char* name)
		{
			if (!json.is_object()) return nullptr;

			auto it = json.find(name);
			if (it == json.end()) return nullptr;

			return &(*it);
		}
	}

	bool LoadFile(const std::string& path, Json& out)
	{
		std::ifstream file(std::filesystem::path(StringUtility::ToWide(path)));
		if (!file)
		{
			Log::Debug("Json: File not found: {}", path);
			return false;
		}

		Json json = Json::parse(file, nullptr, false);
		if (json.is_discarded())
		{
			Log::Warning("Json: Failed to parse: {}", path);
			return false;
		}

		out = std::move(json);
		return true;
	}

	bool SaveFile(const std::string& path, const Json& json)
	{
		std::filesystem::path p = std::filesystem::path(StringUtility::ToWide(path));
		if (p.has_parent_path())
		{
			std::filesystem::create_directories(p.parent_path());
		}

		std::ofstream file(p, std::ios::out | std::ios::trunc);
		if (!file)
		{
			Log::Warning("Json: Failed to open: {}", path);
			return false;
		}

		file << json.dump(4, ' ', false, Json::error_handler_t::replace) << '\n';
		return file.good();
	}

	bool GetBool(const Json& json, const char* name, bool& out)
	{
		const Json* value = Find(json, name);
		if (!value || !value->is_boolean()) return false;

		out = value->get<bool>();
		return true;
	}

	bool GetInt(const Json& json, const char* name, int32_t& out)
	{
		const Json* value = Find(json, name);
		if (!value || !value->is_number_integer()) return false;

		out = value->get<int32_t>();
		return true;
	}

	bool GetUint(const Json& json, const char* name, uint32_t& out)
	{
		const Json* value = Find(json, name);
		if (!value || !value->is_number_unsigned()) return false;

		out = value->get<uint32_t>();
		return true;
	}

	bool GetFloat(const Json& json, const char* name, float& out)
	{
		const Json* value = Find(json, name);
		if (!value || !value->is_number()) return false;

		out = value->get<float>();
		return true;
	}

	bool GetString(const Json& json, const char* name, std::string& out)
	{
		const Json* value = Find(json, name);
		if (!value || !value->is_string()) return false;

		out = value->get<std::string>();
		return true;
	}
}
