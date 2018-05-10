#include <octoon/io/http_stream.h>

namespace octoon
{
	namespace io
	{
		http_reader::http_reader() noexcept
			: istream(&http_)
		{
		}

		http_reader::http_reader(const char* path, const ios_base::open_mode mode) noexcept
			: istream(&http_)
		{
			this->open(path, mode);
		}

		http_reader::http_reader(const std::string& path, const ios_base::open_mode mode) noexcept
			: istream(&http_)
		{
			this->open(path, mode);
		}

		http_reader::~http_reader() noexcept
		{
			this->close();
		}

		bool
		http_reader::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		http_reader&
		http_reader::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!http_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		http_reader&
		http_reader::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		http_reader&
		http_reader::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!http_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		http_write::http_write() noexcept
			: ostream(&http_)
		{
		}

		http_write::http_write(const char* path, const ios_base::open_mode mode) noexcept
			: ostream(&http_)
		{
			this->open(path, mode);
		}

		http_write::http_write(const std::string& path, const ios_base::open_mode mode) noexcept
			: ostream(&http_)
		{
			this->open(path, mode);
		}

		http_write::~http_write() noexcept
		{
			this->close();
		}

		bool
		http_write::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		http_write&
		http_write::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!http_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		http_write&
		http_write::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		http_write&
		http_write::close() noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!http_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		http_stream::http_stream() noexcept
			: iostream(&http_)
		{
		}

		http_stream::http_stream(const char* path, const ios_base::open_mode mode) noexcept
			: iostream(&http_)
		{
			this->open(path, mode);
		}

		http_stream::http_stream(const std::string& path, const ios_base::open_mode mode) noexcept
			: iostream(&http_)
		{
			this->open(path, mode);
		}

		http_stream::~http_stream() noexcept
		{
			this->close();
		}

		http_stream&
		http_stream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!http_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		http_stream&
		http_stream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		bool
		http_stream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		http_stream&
		http_stream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!http_.close())
					this->setstate(failbit);
			}

			return (*this);
		}
	}
}