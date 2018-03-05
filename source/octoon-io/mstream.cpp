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
		membuf::membuf() noexcept
		{
		}

		membuf::membuf(std::size_t capacity) noexcept
		{
			this->open(capacity);
		}

		membuf::membuf(std::vector<std::uint8_t>&& buffer) noexcept
		{
			this->open(std::move(buffer));
		}

		membuf::membuf(const std::vector<std::uint8_t>& buffer) noexcept
		{
			this->open(buffer);
		}

		membuf::~membuf() noexcept
		{
		}

		bool
		membuf::is_open() const noexcept
		{
			return !buffer_.empty();
		}

		void
		membuf::open(std::size_t capacity) noexcept
		{
			buffer_.resize(capacity);
		}

		void
		membuf::open(std::vector<std::uint8_t>&& buffer) noexcept
		{
			buffer_ = std::move(buffer_);
		}

		void
		membuf::open(const std::vector<std::uint8_t>& buffer) noexcept
		{
			buffer_ = buffer;
		}

		streamsize
		membuf::read(char* str, std::streamsize cnt) noexcept
		{
			std::lock_guard<std::mutex> guard(lock_);
			auto buf_size = buffer_.size();
			std::size_t actual_copyable;

			// There is enough bytes to write.
			if (buf_size - pos_ > cnt) {
				actual_copyable = cnt;
				std::memcpy(str, buffer_.data() + pos_, actual_copyable);
				pos_ += cnt;

				// Bytes not enough, write all we can have.
			}
			else {
				actual_copyable = buf_size - pos_;
				std::memcpy(str, buffer_.data() + pos_, actual_copyable);
				pos_ = buf_size;
			}
			return actual_copyable;
		}

		streamsize
		membuf::write(const char* str, std::streamsize cnt) noexcept
		{
			std::lock_guard<std::mutex> guard(lock_);

			// Re-allocation is required to store incoming data.
			buffer_.reserve(pos_ + cnt);
			std::memcpy(buffer_.data() + pos_, str, cnt);
			return cnt;
		}

		streamoff
		membuf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			size_t base = 0;
			switch (dir)
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

			std::size_t resultant = base + pos;
			if (resultant >= 0 && resultant <= buffer_.size())
			{
				pos_ = resultant;
				return true;
			}

			return false;
		}

		streamoff
		membuf::tellg() noexcept
		{
			return pos_;
		}

		streamsize
		membuf::size() const noexcept
		{
			return buffer_.size();
		}

		int
		membuf::flush() noexcept
		{
			return 0;
		}

		bool
		membuf::close() noexcept
		{
			buffer_.clear();
			return true;
		}

		mstream::mstream() noexcept
			: istream(&buf_)
		{
		}

		mstream::mstream(std::size_t capacity, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(capacity, mode);
		}

		mstream::mstream( std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(std::move(buffer), mode);
		}

		mstream::mstream(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(buffer, mode);
		}

		bool
		mstream::is_open() const noexcept
		{
			return buf_.is_open();
		}

		mstream&
		mstream::open(std::size_t capacity, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(capacity);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		mstream&
		mstream::open(std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(std::move(buffer));
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		mstream&
		mstream::open(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(buffer);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		mstream&
		mstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!buf_.close())
					this->setstate(failbit);
			}

			return (*this);
		}
	}
}