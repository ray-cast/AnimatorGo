// File: virtual_dirs.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_VIRTUAL_DIRS_H_
#define OCTOON_IO_VIRTUAL_DIRS_H_

#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		/*
		* Zip package as a virtual directory.
		*
		* **NOTE** Zip archives are always read-only. Any non-read options set true
		* will lead to rejection.
		*/
		class OCTOON_EXPORT zpackage : public package
		{
		public:
			zpackage(const char* zip_file) except;
			zpackage(std::string&& zip_file) except;
			zpackage(const std::string& zip_file) except;
			~zpackage() noexcept;

			std::unique_ptr<stream_buf> open(const Orl& orl, const ios_base::open_mode options) override;
			bool remove(const Orl& orl, ios_base::file_type type = ios_base::file) override;
			ios_base::file_type exists(const Orl& orl) override;

		private:

			// Hide unzipper.
			void* unzipper_;
			void* entries_;
		};

		using ZipArchivePtr = std::shared_ptr<zpackage>;
	}
}

#endif