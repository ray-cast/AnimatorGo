// File: mstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <utility>
#include <cstring>
#include <octoon/io/mstream.h>

namespace octoon
{
	namespace io
	{
		constexpr size_t DEFAULT_BUFFER_SIZE = 8 * 1024;

		mstream::mstream() noexcept
			: buffer_(DEFAULT_BUFFER_SIZE)
		{
		}

		mstream::mstream(mstream&& rv) noexcept
			: buffer_(std::move(rv.buffer_))
			, pos_(rv.pos_)
		{
		}

		mstream::mstream(std::size_t capacity) noexcept
			: buffer_(capacity)
		{
		}

		mstream::mstream(std::vector<std::uint8_t> buffer) noexcept
			: buffer_(buffer)
			, pos_(0)
		{
		}

		mstream&
		mstream::operator=(mstream&& rv) noexcept
		{
			buffer_ = std::move(rv.buffer_);
			pos_ = std::move(rv.pos_);
			return *this;
		}

		bool
		mstream::can_read() const noexcept
		{
			return true;
		}

		bool
		mstream::can_write() const noexcept
		{
			return true;
		}

		bool
		mstream::can_seek() const noexcept
		{
			return true;
		}

		std::size_t
		mstream::read(std::uint8_t* buf, std::size_t size)
		{
			std::lock_guard<std::mutex> guard(lock_);
			auto buf_size = buffer_.size();
			std::size_t actual_copyable;

			// There is enough bytes to write.
			if (buf_size - pos_ > size) {
				actual_copyable = size;
				std::memcpy(buf, buffer_.data() + pos_, actual_copyable);
				pos_ += size;

				// Bytes not enough, write all we can have.
			}
			else {
				actual_copyable = buf_size - pos_;
				std::memcpy(buf, buffer_.data() + pos_, actual_copyable);
				pos_ = buf_size;
			}
			return actual_copyable;
		}

		std::size_t
		mstream::write(const std::uint8_t* buf, std::size_t size)
		{
			std::lock_guard<std::mutex> guard(lock_);

			// Re-allocation is required to store incoming data.
			buffer_.reserve(pos_ + size);
			std::memcpy(buffer_.data() + pos_, buf, size);
			return size;
		}

		bool mstream::seek(long dist, ios_base::seek_dir seek)
		{
			size_t base = 0;
			switch (seek)
			{
			case ios_base::beg:
				base = 0;
				break;
			case ios_base::end:
				base = pos_;
				break;
			case ios_base::cur:
				base = buffer_.size();
				break;
			}

			std::size_t resultant = base + dist;
			if (resultant >= 0 && resultant <= buffer_.size())
			{
				pos_ = resultant;
				return true;
			}

			return false;
		}

		std::vector<std::uint8_t>
		mstream::into_buffer() noexcept
		{
			return std::move(buffer_);
		}
	}
}