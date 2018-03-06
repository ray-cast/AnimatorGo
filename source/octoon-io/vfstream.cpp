#include <octoon/io/vfstream.h>

namespace octoon
{
	namespace io
	{
		ivfstream::ivfstream() noexcept
			: istream(&buf_)
		{
		}

		ivfstream::ivfstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(orl, mode);
		}

		ivfstream::ivfstream(const char* path, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(path, mode);
		}

		ivfstream::ivfstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(path, mode);
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
		ivfstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ivfstream&
		ivfstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

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

		ovfstream::ovfstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(orl, mode);
		}

		ovfstream::ovfstream(const char* path, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(path, mode);
		}

		ovfstream::ovfstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(path, mode);
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
		ovfstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ovfstream&
		ovfstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

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

		vfstream::vfstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(orl, mode);
		}

		vfstream::vfstream(const char* path, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(path, mode);
		}

		vfstream::vfstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(path, mode);
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
		vfstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

				auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

				if (!buf_.open(fs, orl, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		vfstream&
		vfstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				Orl orl;
				Orl::parse(path, orl);

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