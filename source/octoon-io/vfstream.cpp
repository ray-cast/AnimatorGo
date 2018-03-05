// File: vfstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <octoon/io/vfstream.h>

namespace octoon
{
	namespace io
	{
		vfstream::vfstream() noexcept
			: fs_(IoServer::instance())
		{
		}

		vfstream::vfstream(std::shared_ptr<IoServer> fs) noexcept
			: fs_(fs)
		{
		}

		vfstream::vfstream(vfstream&& rv) noexcept
			: fs_(std::move(rv.fs_))
		{
		}

		vfstream&
		vfstream::operator=(vfstream&& rv) noexcept
		{
			fs_ = std::move(rv.fs_);
			return *this;
		}

		bool
		vfstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			this->close();

			auto vdir = fs_->get_archive(orl);
			if (vdir == nullptr)
				return false;

			stream_ = vdir->open(orl, mode);
			return stream_ != nullptr;
		}

		void
		vfstream::close() noexcept
		{
			stream_.reset();
		}
	}
}