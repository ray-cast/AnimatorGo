#ifndef OCTOON_FILE_MODULE_H_
#define OCTOON_FILE_MODULE_H_

#include <mysticLit_model.h>
#include <vector>

namespace MysticLit
{
	class FileModule : public MysticLitModule
	{
	public:
		FileModule() noexcept;
		virtual ~FileModule() noexcept;

		virtual void reset() noexcept override;

	private:
		FileModule(const FileModule&) = delete;
		FileModule& operator=(const FileModule&) = delete;

	public:
		std::uint32_t PATHLIMIT;

		std::vector<const char*> projectExtensions;
		std::vector<const char*> modelExtensions;
		std::vector<const char*> imageExtensions;
		std::vector<const char*> videoExtensions;
	};
}

#endif