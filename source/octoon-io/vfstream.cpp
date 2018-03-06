#include <octoon/io/vfstream.h>

namespace octoon
{
	namespace io
	{
		ivfstream::ivfstream() noexcept
			: istream(&buf_)
		{
		}

		ivfstream::ivfstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: istream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ivfstream::ivfstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: istream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ivfstream&
		ivfstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ivfstream&
		ivfstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		ivfstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		ovfstream::ovfstream() noexcept
			: ostream(&buf_)
		{
		}

		ovfstream::ovfstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: ostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ovfstream::ovfstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: ostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ovfstream&
		ovfstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ovfstream&
		ovfstream::close() noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		ovfstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		vfstream::vfstream() noexcept
			: iostream(&buf_)
		{
		}

		vfstream::vfstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: iostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		vfstream::vfstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: iostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		vfstream&
		vfstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		vfstream&
		vfstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		vfstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}
	}
}