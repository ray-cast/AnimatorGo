// File: fstream.cpp
// Author: PENGUINLIONG

#include <octoon/io/fstream.h>

namespace octoon
{
	namespace io
	{
		bool
		fstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
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

		bool
		fstream::can_read() const noexcept
		{
			return opts_ & ios_base::in;
		}

		bool
		fstream::can_write() const noexcept
		{
			return opts_ & ios_base::out;
		}

		bool
		fstream::can_seek() const noexcept
		{
			return true;
		}

		std::size_t
		fstream::read(uint8_t* buf, std::size_t size) noexcept
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

		std::size_t
		fstream::write(const uint8_t* buf, std::size_t size) noexcept
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

		bool
		fstream::seek(long dist, ios_base::seek_dir seek) noexcept
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
	}
}