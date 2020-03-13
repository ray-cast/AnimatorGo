#include <octoon/io/iostream.h>

namespace octoon
{
	namespace io
	{
		iostream::iosentry::iosentry(iostream* _istr)
			: _ok(true)
			, _myIostr(_istr)
		{
			if (_myIostr->rdbuf() != 0)
				_myIostr->rdbuf()->lock();
		}

		iostream::iosentry::~iosentry() noexcept
		{
			if (_myIostr->rdbuf() != 0)
				_myIostr->rdbuf()->unlock();
		}

		iostream::iosentry::operator bool() const noexcept
		{
			return _ok ? true : false;
		}

		iostream::iostream(stream_buf* buf) noexcept
			: istream(buf)
			, ostream(buf)
			, _count(0)
		{
			stream::_init(buf, ios_base::in | ios_base::out);
		}

		iostream::~iostream() noexcept
		{
		}

		iostream&
		iostream::read(char* str, std::streamsize cnt) noexcept
		{
			assert(cnt != 0);

			try
			{
				ios_base::iostate state = ios_base::goodbit;

				const iosentry ok(this);

				if (ok)
				{
					_count = this->rdbuf()->read(str, cnt);

					if (_count != cnt)
						state |= ios_base::badbit;
				}

				this->setstate(state);
			}
			catch (...)
			{
				this->setstate(ios_base::badbit, true);
			}

			return (*this);
		}

		iostream&
		iostream::read(char* str, streamsize size, streamsize cnt) noexcept
		{
			return this->read(str, size * cnt);
		}

		iostream&
		iostream::write(const char* str, std::streamsize cnt) noexcept
		{
			assert(cnt != 0);

			try
			{
				ios_base::iostate state = ios_base::goodbit;

				const iosentry ok(this);

				if (ok)
				{
					if (this->rdbuf()->write(str, cnt) != cnt)
						state |= ios_base::badbit;
				}

				this->setstate(state);
			}
			catch (...)
			{
				this->setstate(ios_base::badbit, true);
			}

			return (*this);
		}

		iostream&
		iostream::write(const char* str, streamsize size, streamsize cnt) noexcept
		{
			return this->write(str, size * cnt);
		}

		iostream&
		iostream::flush() noexcept
		{
			if (this->rdbuf() != 0)
			{
				const iosentry ok(this);
				if (ok)
				{
					if (ok && this->rdbuf()->flush() == -1)
						this->setstate(ios_base::badbit, true);
				}
			}

			return (*this);
		}

		iostream&
		iostream::seekg(ios_base::off_type pos) noexcept
		{
			const iosentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		iostream&
		iostream::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			const iosentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		streamsize
		iostream::size() noexcept
		{
			const iosentry ok(this);
			if (ok)
			{
				if (!this->fail())
					return this->rdbuf()->size();
			}

			return (streamsize)ios_base::_BADOFF;
		}

		streamsize
		iostream::gcount() const noexcept
		{
			return _count;
		}

		streampos
		iostream::tellg() noexcept
		{
			const iosentry ok(this);

			if (!this->fail())
				return (this->rdbuf()->tellg());
			else
				return (ios_base::pos_type(ios_base::_BADOFF));
		}
	}
}