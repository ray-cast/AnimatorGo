// File: mstream.cpp
// Author: PENGUINLIONG
#include <cassert>
#include <utility>
#include <cstring>
#include "octoon/io/mstream.h"

namespace octoon
{
	namespace io
	{
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

		mstream::mstream(std::vector<uint8_t> buffer) noexcept
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
		mstream::can_read() noexcept
		{
			return true;
		}

		bool
		mstream::can_write() noexcept
		{
			return true;
		}

		bool
		mstream::can_seek() noexcept
		{
			return true;
		}

		std::size_t
		mstream::read(uint8_t* buf, size_t size)
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
		mstream::write(const uint8_t* buf, size_t size)
		{
			std::lock_guard<std::mutex> guard(lock_);

			// Re-allocation is required to store incoming data.
			buffer_.reserve(pos_ + size);
			std::memcpy(buffer_.data() + pos_, buf, size);
			return size;
		}

		bool mstream::seek(long dist, SeekOrigin ori)
		{
			size_t base = 0;
			switch (ori)
			{
			case octoon::io::SeekOrigin::Start:
				base = 0;
				break;
			case octoon::io::SeekOrigin::End:
				base = pos_;
				break;
			case octoon::io::SeekOrigin::Current:
				base = buffer_.size();
				break;
			}
			size_t resultant = base + dist;
			if (resultant >= 0 && resultant <= buffer_.size()) {
				pos_ = resultant;
				return true;
			}
			return false;
		}

		std::vector<uint8_t>
		mstream::into_buffer() noexcept
		{
			return std::move(buffer_);
		}
	}
}