// File: virtual_dirs.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FARCHIVE_H_
#define OCTOON_IO_FARCHIVE_H_

#include <octoon/io/istream.h>
#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		/*
		* Local directory mapped directly to a virtual directory.
		*/
		class OCTOON_EXPORT fpackage final : public package
		{
		public:
			fpackage(const char* base_dir) noexcept;
			fpackage(std::string&& base_dir) noexcept;
			fpackage(const std::string& base_dir) noexcept;
			~fpackage() noexcept = default;

			std::unique_ptr<stream_buf> open(const Orl& orl, const ios_base::open_mode mode) override;

			bool remove(const Orl& orl, ios_base::file_type type = ios_base::file) override;
			ios_base::file_type exists(const Orl& orl) override;

		private:
			std::string make_path(const Orl& orl) const;
			std::string base_dir_;
		};

		using LocalDirPtr = std::shared_ptr<fpackage>;
	}
}

#endif