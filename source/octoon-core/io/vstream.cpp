#include <octoon/io/vstream.h>

namespace octoon
{
	namespace io
	{
		ivstream::ivstream() noexcept
			: istream(&buf_)
		{
		}

		ivstream::ivstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(orl, mode);
		}

		ivstream::ivstream(const char* path, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(path, mode);
		}

		ivstream::ivstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: istream(&buf_)
		{
			this->open(path, mode);
		}

		ivstream::ivstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: istream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ivstream::ivstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: istream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ivstream&
		ivstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
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

		ivstream&
		ivstream::open(const char* path, const ios_base::open_mode mode) noexcept
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

		ivstream&
		ivstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				Orl orl;
				if (Orl::parse(path, orl))
				{
					auto fs = filesystem_ ? filesystem_.get() : IoServer::instance();

					if (!buf_.open(fs, orl, mode))
						this->setstate(ios_base::failbit, mode);
					else
						this->clear(ios_base::goodbit, mode);
				}
				else
				{
					if (!buf_.open(path, mode))
						this->setstate(ios_base::failbit, mode);
					else
						this->clear(ios_base::goodbit, mode);
				}

			}

			return (*this);
		}

		ivstream&
		ivstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		ivstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		ovstream::ovstream() noexcept
			: ostream(&buf_)
		{
		}

		ovstream::ovstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(orl, mode);
		}

		ovstream::ovstream(const char* path, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(path, mode);
		}

		ovstream::ovstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: ostream(&buf_)
		{
			this->open(path, mode);
		}

		ovstream::ovstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: ostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ovstream::ovstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: ostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		ovstream&
		ovstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
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

		ovstream&
		ovstream::open(const char* path, const ios_base::open_mode mode) noexcept
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

		ovstream&
		ovstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
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

		ovstream&
		ovstream::close() noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		ovstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		vstream::vstream() noexcept
			: iostream(&buf_)
		{
		}

		vstream::vstream(const Orl& orl, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(orl, mode);
		}

		vstream::vstream(const char* path, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(path, mode);
		}

		vstream::vstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: iostream(&buf_)
		{
			this->open(path, mode);
		}

		vstream::vstream(std::shared_ptr<IoServer>&& filesystem) noexcept
			: iostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		vstream::vstream(const std::shared_ptr<IoServer>& filesystem) noexcept
			: iostream(&buf_)
			, filesystem_(filesystem)
		{
		}

		vstream&
		vstream::open(const Orl& orl, const ios_base::open_mode mode) noexcept
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

		vstream&
		vstream::open(const char* path, const ios_base::open_mode mode) noexcept
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

		vstream&
		vstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
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

		vstream&
		vstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				buf_.close();
			}

			return (*this);
		}

		bool
		vstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}
	}
}