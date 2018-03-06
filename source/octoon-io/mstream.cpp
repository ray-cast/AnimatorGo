// File: omstream.cpp
// Author: PENGUINLIONG
#include <octoon/io/mstream.h>

namespace octoon
{
	namespace io
	{
		imstream::imstream() noexcept
			: istream(&buf_)
		{
		}

		imstream::imstream(std::size_t capacity, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(capacity, mode);
		}

		imstream::imstream( std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(std::move(buffer), mode);
		}

		imstream::imstream(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(buffer, mode);
		}

		bool
		imstream::is_open() const noexcept
		{
			return buf_.is_open();
		}

		imstream&
		imstream::open(std::size_t capacity, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(capacity);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		imstream&
		imstream::open(std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(std::move(buffer));
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		imstream&
		imstream::open(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.open(buffer);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		imstream&
		imstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!buf_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		omstream::omstream() noexcept
			: ostream(&buf_)
		{
		}

		omstream::omstream(std::size_t capacity, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(capacity, mode);
		}

		omstream::omstream(std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(std::move(buffer), mode);
		}

		omstream::omstream(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(buffer, mode);
		}

		bool
		omstream::is_open() const noexcept
		{
			return buf_.is_open();
		}

		omstream&
		omstream::open(std::size_t capacity, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				buf_.open(capacity);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		omstream&
		omstream::open(std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				buf_.open(std::move(buffer));
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		omstream&
		omstream::open(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				buf_.open(buffer);
				this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		omstream&
		omstream::close() noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!buf_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		mstream::mstream() noexcept
			: iostream(&buf_)
		{
		}

		mstream::mstream(std::size_t capacity, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(capacity, mode);
		}

		mstream::mstream(std::vector<std::uint8_t>&& buffer, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(std::move(buffer), mode);
		}

		mstream::mstream(const std::vector<std::uint8_t>& buffer, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
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
			const osentry ok(this);
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
			const osentry ok(this);
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
			const osentry ok(this);
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
			const osentry ok(this);
			if (ok)
			{
				if (!buf_.close())
					this->setstate(failbit);
			}

			return (*this);
		}
	}
}