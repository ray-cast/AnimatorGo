#include <octoon/io/istream.h>

namespace octoon
{
	namespace io
	{
		istream::isentry::isentry(istream* _istr)
			: _ok(true)
			, _my_istr(_istr)
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->lock();
		}

		istream::isentry::~isentry() noexcept
		{
			if (_my_istr->rdbuf() != 0)
				_my_istr->rdbuf()->unlock();
		}

		istream::isentry::operator bool() const noexcept
		{
			return _ok ? true : false;
		}

		istream::istream(stream_buf* buf) noexcept
			: _count(0)
		{
			stream::_init(buf, ios_base::in);
		}

		istream::~istream() noexcept
		{
		}

		istream&
		istream::read(char* str, std::streamsize cnt) noexcept
		{
			try
			{
				if (cnt == 0)
					return *this;

				ios_base::iostate state = ios_base::goodbit;

				const isentry ok(this);

				if (ok)
				{
					_count = this->rdbuf()->read(str, cnt);

					if (_count != cnt)
						state |= ios_base::eofbit;
				}

				this->setstate(state);
			}
			catch (...)
			{
				this->setstate(ios_base::badbit, true);
			}

			return (*this);
		}

		istream&
		istream::read(char* str, streamsize size, streamsize cnt) noexcept
		{
			return this->read(str, size * cnt);
		}

		istream&
		istream::flush() noexcept
		{
			assert(this->rdbuf() != 0);

			const isentry ok(this);
			if (ok)
			{
				if (ok && this->rdbuf()->flush() == -1)
					this->setstate(ios_base::badbit, true);
			}

			return (*this);
		}

		istream&
		istream::seekg(ios_base::off_type pos) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		istream&
		istream::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		streamsize
		istream::size() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!this->fail())
					return this->rdbuf()->size();
			}

			return (streamsize)ios_base::_BADOFF;
		}

		streamoff
		istream::tellg() noexcept
		{
			const isentry ok(this);

			if (!this->fail())
				return (this->rdbuf()->tellg());
			else
				return (ios_base::pos_type(ios_base::_BADOFF));
		}

		streamsize
		istream::gcount() const noexcept
		{
			return _count;
		}

		istream&
		istream::operator >> (std::string& str) noexcept
		{
			const isentry _Ok(this);
			if (_Ok)
			{
				auto written = this->rdbuf()->size() - this->rdbuf()->tellg();
				auto size = str.size();
				str.resize(size + written);
				return this->read((char*)&str[size], written);
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		istream&
		istream::operator >> (ios_base& (*function)(ios_base&)) noexcept
		{
			assert(function);
			(*function)(*(ios_base *)this);
			return (*this);
		}

		istream&
		istream::operator >> (istream& (*function)(istream&)) noexcept
		{
			assert(function);
			(*function)(*this);
			return (*this);
		}
	}
}