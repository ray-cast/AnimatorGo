#ifndef OCTOON_ASSET_IMPORTER_CONTEXT_H_
#define OCTOON_ASSET_IMPORTER_CONTEXT_H_

#include <octoon/runtime/object.h>
#include <octoon/runtime/json.h>
#include <filesystem>
#include <set>

namespace octoon
{
	class OCTOON_EXPORT AssetImporterContext final
	{
	public:
		AssetImporterContext(const std::filesystem::path& path) noexcept;
		virtual ~AssetImporterContext() noexcept;

		void setMainObject(const std::shared_ptr<Object>& object) noexcept;
		const std::shared_ptr<Object>& getMainObject() const noexcept;

		void addObjectToAsset(std::string_view identifier, const std::shared_ptr<Object>& object);
		const std::vector<std::shared_ptr<Object>>& getObjects() const;

		const std::filesystem::path& getAssetPath() const noexcept;

		nlohmann::json getMetadata() const noexcept(false);

	private:
		AssetImporterContext(const AssetImporterContext&) = delete;
		AssetImporterContext& operator=(const AssetImporterContext&) = delete;

	private:
		nlohmann::json metaData_;
		std::filesystem::path assetPath_;
		std::shared_ptr<Object> mainObject;
		std::map<std::string, std::set<std::string>> identifiers_;
		std::vector<std::shared_ptr<Object>> subAssets_;
	};
}

#endif