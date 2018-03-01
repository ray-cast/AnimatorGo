// File: fstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <octoon/io/fstream.h>
#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		fstream::fstream() noexcept
			: fs_(IoServer::instance())
		{
		}

		fstream::fstream(std::shared_ptr<IoServer> fs) noexcept
			: fs_(fs)
		{
		}

		fstream::fstream(fstream&& rv) noexcept
			: fs_(std::move(rv.fs_))
		{
		}

		fstream&
		fstream::operator=(fstream&& rv) noexcept
		{
			fs_ = std::move(rv.fs_);
			return *this;
		}

		bool
		fstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			this->close();

			auto vdir = fs_->get_archive(orl);
			if (vdir == nullptr)
				return false;

			stream_ = vdir->open(orl, mode);
			return stream_ != nullptr;
		}

		void
		fstream::close() noexcept
		{
			stream_.reset();
		}

		bool
		fstream::can_read() const noexcept
		{
			return stream_ != nullptr && stream_->can_read();
		}

		bool
		fstream::can_write() const noexcept
		{
			return stream_ != nullptr && stream_->can_write();
		}

		bool
		fstream::can_seek() const noexcept
		{
			return stream_ != nullptr && stream_->can_seek();
		}

		std::size_t
		fstream::read(uint8_t* buf, std::size_t size)
		{
			return stream_->read(buf, size);
		}

		std::size_t
		fstream::write(const uint8_t* buf, std::size_t size)
		{
			return stream_->write(buf, size);
		}

		bool
		fstream::seek(long dist, ios_base::seek_dir seek)
		{
			return stream_->seek(dist, seek);
		}
	}
}