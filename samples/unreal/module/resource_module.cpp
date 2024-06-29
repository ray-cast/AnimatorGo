#include "resource_module.h"
#include <shlobj.h>
#include <filesystem>

namespace unreal
{
	ResourceModule::ResourceModule() noexcept
	{
		this->reset();
	}

	ResourceModule::~ResourceModule() noexcept
	{
	}

	void
	ResourceModule::reset() noexcept
	{
#ifdef _WINDOWS_
		wchar_t path[MAX_PATH];
		if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK)
			this->rootPath = std::filesystem::path(path).append(L".animator");
		else
			this->rootPath = L"../../system";

		this->cachePath = std::filesystem::path(this->rootPath).append(L"cache");
#else
		this->rootPath = L"../../system";
		this->cachePath = L"../../system/cache";
#endif
	}

	void 
	ResourceModule::load(nlohmann::json& reader) noexcept
	{
	}

	void 
	ResourceModule::save(nlohmann::json& writer) noexcept
	{
	}
}