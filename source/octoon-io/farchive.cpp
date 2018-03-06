// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <fstream>
#include <filesystem>
#include <cassert>

#include <octoon/io/farchive.h>
#include <octoon/io/mstream.h>
#include <octoon/io/fstream.h>

namespace std
{
	namespace filesystem
	{
		using namespace std::experimental::filesystem;
	}
}

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
			auto file_path = make_path(orl);
			auto parent = orl.parent();

			// Check if the file exists.
			if (exists(orl) == ItemType::NA)
			{
				// Make sure it can create files and directories.
				if (!(opts | ios_base::out) || !(opts | ios_base::trunc)) {
					return nullptr;
				}

				// Create missing segments.
				if (exists(parent) == ItemType::NA && !std::filesystem::create_directories(make_path(parent))) {
					return nullptr;
				}
			}

			// Open the file.
			if (file->open(make_path(orl), opts))
				return file;
			else
				return nullptr;
		}

		bool
		farchive::remove(const Orl& orl, ItemType type)
		{
			auto path = make_path(orl);
			auto status = std::filesystem::status(path).type();
			if (status == std::filesystem::file_type::not_found) {
				return false;
			}
			if (type == ItemType::File &&
				status == std::filesystem::file_type::regular) {
				return std::filesystem::remove(path);
			}
			if (type == ItemType::Directory &&
				status == std::filesystem::file_type::directory) {
				return std::filesystem::remove_all(path);
			}
			return false;
		}

		ItemType
		farchive::exists(const Orl& orl)
		{
			auto status = std::filesystem::status(make_path(orl));
			switch (status.type())
			{
			case std::filesystem::file_type::regular:
				return ItemType::File;
			case std::filesystem::file_type::directory:
				return ItemType::Directory;
			default:
				return ItemType::NA;
			}
		}

		std::string
		farchive::make_path(const Orl& orl) const
		{
			std::string rv;
			auto path = orl.path();
			if (path.size() > 0 && path.back() != '/')
			{
				rv.reserve(base_dir_.size() + 1 + path.size());
				rv.append(base_dir_);
				rv.push_back('/');
				rv.append(path);
			}
			else
			{
				rv.reserve(base_dir_.size() + path.size());
				rv.append(base_dir_);
				rv.append(path);
			}

			return rv;
		}
	}
} 