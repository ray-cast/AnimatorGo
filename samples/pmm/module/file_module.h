#ifndef MYSTICLIT_FILE_MODULE_H_
#define MYSTICLIT_FILE_MODULE_H_

#include <mysticLit_model.h>
#include <vector>

namespace MysticLit
{
	class FileModule final : public MysticLitModule
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