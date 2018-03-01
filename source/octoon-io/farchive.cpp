// File: virtual_dirs.h
// Author: PENGUINLIONG
#include <fstream>
#include <filesystem>
#include <cassert>

#include <octoon/io/farchive.h>
#include <octoon/io/mstream.h>

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
		namespace detail
		{
			class LocalFileStream : public stream
			{
			public:
				bool open(const std::string& path, const ios_base::open_mode mode) noexcept
				{
					std::ios_base::open_mode openmode = std::ios_base::binary;
					if (mode & ios_base::in)
						openmode |= std::ios_base::in;

					if (mode & ios_base::out)
						openmode |= std::ios_base::out;

					if (mode & ios_base::app)
						openmode |= std::ios_base::app;

					if (mode & ios_base::trunc)
						openmode |= std::ios_base::trunc;

					stream_.open(path, openmode);
					if (!stream_.is_open())
						return false;

					opts_ = mode;
					return true;
				}

				bool can_read() const noexcept
				{
					return opts_ & ios_base::in;
				}

				bool can_write() const noexcept
				{
					return opts_ & ios_base::out;
				}

				bool can_seek() const noexcept
				{
					return true;
				}

				std::size_t read(uint8_t* buf, std::size_t size) noexcept
				{
					try
					{
						stream_.read((char*)buf, size);
					}
					catch (const std::ios_base::failure&)
					{
						return 0;
					}

					stream_.seekp(stream_.tellg(), std::ios_base::beg);
					return stream_.gcount();
				}

				std::size_t write(const uint8_t* buf, std::size_t size) noexcept
				{
					try
					{
						stream_.write((char*)buf, size);
					}
					catch (const std::ios_base::failure&)
					{
						return 0;
					}

					stream_.seekg(stream_.tellp(), std::ios_base::beg);
					return size;
				}

				bool seek(long dist, ios_base::seek_dir seek) noexcept
				{
					try
					{
						switch (seek)
						{
						case ios_base::beg:
							stream_.seekg(dist, std::ios_base::beg);
							stream_.seekp(dist, std::ios_base::beg);
							break;
						case ios_base::end:
							stream_.seekg(dist, std::ios_base::end);
							stream_.seekp(dist, std::ios_base::end);
							break;
						case ios_base::cur:
							stream_.seekg(dist, std::ios_base::cur);
							stream_.seekp(dist, std::ios_base::cur);
							break;
						}
						return true;
					}
					catch (const std::ios_base::failure&)
					{
						return false;
					}
				}
			private:
				std::fstream stream_;
				ios_base::open_mode opts_;
			};
		}

		farchive::farchive(const std::string& base_dir) noexcept
			: base_dir_(base_dir)
		{
		}

		std::unique_ptr<stream>
		farchive::open(const Orl& orl, const ios_base::open_mode opts)
		{
			auto rv_stream = std::make_unique<detail::LocalFileStream>();
			auto parent = orl.parent();
			auto file_path = make_path(orl);

			// Check if the file exists.
			if (exists(orl) == ItemType::NA)
			{
				// Make sure it can create files and directories.
				if (!(opts | ios_base::out) || !(opts | ios_base::trunc)) {
					return nullptr;
				}

				// Create missing segments.
				if (exists(parent) == ItemType::NA &&
					!std::filesystem::create_directories(make_path(parent))) {
					return nullptr;
				}
			}

			// Open the file.
			if (rv_stream->open(make_path(orl), opts)) {
				return rv_stream;
			}
			else {
				return nullptr;
			}
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