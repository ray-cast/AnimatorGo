#ifndef UNREAL_RESOURCE_MODULE_H_
#define UNREAL_RESOURCE_MODULE_H_

#include <unreal_model.h>
#include <filesystem>

namespace unreal
{
	class ResourceModule final : public UnrealModule
	{
	public:
		ResourceModule() noexcept;
		virtual ~ResourceModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

	private:
		ResourceModule(const ResourceModule&) = delete;
		ResourceModule& operator=(const ResourceModule&) = delete;

	public:
		std::filesystem::path rootPath;
		std::filesystem::path cachePath;
	};
}

#endif