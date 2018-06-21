// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <fstream>
#include <cassert>

#include <octoon/io/farchive.h>
#include <octoon/io/mstream.h>
#include <octoon/io/fstream.h>

#ifdef _MSC_VER
#include <filesystem>
namespace std
{
	namespace filesystem
	{
		using namespace std::experimental::filesystem;
	}
}
#endif

namespace octoon
{
	namespace io
	{
		farchive::farchive(const char* base_dir) noexcept
			: base_dir_(base_dir)
		{
		}

		farchive::farchive(std::string&& base_dir) noexcept
			: base_dir_(std::move(base_dir))
		{
		}

		farchive::farchive(const std::string& base_dir) noexcept
			: base_dir_(base_dir)
		{
		}

		std::unique_ptr<stream_buf>
		farchive::open(const Orl& orl, const ios_base::open_mode opts)
		{
			auto file = std::make_unique<filebuf>();

			// Open the file.
			if (file->open(make_path(orl), opts))
				return file;
			else
				return nullptr;
		}

		bool
		farchive::remove(const Orl& orl, ios_base::file_type type)
		{
#ifdef _MSC_VER
			auto path = make_path(orl);
			auto status = std::filesystem::status(path).type();
			if (status == std::filesystem::file_type::not_found) {
				return false;
			}
			if (type == ios_base::file &&
				status == std::filesystem::file_type::regular) {
				return std::filesystem::remove(path);
			}
			if (type == ios_base::directory &&
				status == std::filesystem::file_type::directory) {
				return std::filesystem::remove_all(path);
			}
			return false;
#else
			return false;
#endif
		}

		ios_base::file_type
		farchive::exists(const Orl& orl)
		{
#ifdef _MSC_VER
			auto status = std::filesystem::status(make_path(orl));
			switch (status.type())
			{
			case std::filesystem::file_type::regular:
				return ios_base::file;
			case std::filesystem::file_type::directory:
				return ios_base::directory;
			default:
				return ios_base::none;
			}
#else
			return ios_base::none;
#endif
		}

		std::string
		farchive::make_path(const Orl& orl) const
		{
			std::string rv;
			rv.reserve(base_dir_.size() + orl.path().size());
			rv.append(base_dir_);
			rv.append(orl.path());

			return rv;
		}
	}
}