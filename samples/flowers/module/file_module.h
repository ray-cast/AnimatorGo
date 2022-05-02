#ifndef FLOWER_FILE_MODULE_H_
#define FLOWER_FILE_MODULE_H_

#include <flower_model.h>
#include <vector>

namespace flower
{
	class FileModule final : public FlowerModule
	{
	public:
		FileModule() noexcept;
		virtual ~FileModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		FileModule(const FileModule&) = delete;
		FileModule& operator=(const FileModule&) = delete;

	public:
		std::uint32_t PATHLIMIT;

		std::vector<const char*> projectExtensions;
		std::vector<const char*> modelExtensions;
		std::vector<const char*> imageExtensions;
		std::vector<const char*> hdriExtensions;
		std::vector<const char*> videoExtensions;
	};
}

#endif