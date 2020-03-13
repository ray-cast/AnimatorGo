#include <octoon/io/ostream.h>
#include <octoon/runtime/string.h>

#include <stdlib.h>
#include <stdio.h>
#include <cstring>

namespace octoon
{
	namespace io
	{
		ostream::osentry::osentry(ostream* _ostr)
			:_my_ostr(_ostr)
		{
			if (_my_ostr->rdbuf() != 0)
				_my_ostr->rdbuf()->lock();
			_ok = _ostr->good();
		}

		ostream::osentry::~osentry() noexcept
		{
			if (_my_ostr->rdbuf() != 0)
				_my_ostr->rdbuf()->unlock();
		}

		ostream::osentry::operator bool() const noexcept
		{
			return (_ok ? true : false);
		}

		ostream::ostream(stream_buf* buf) noexcept
		{
			stream::_init(buf, ios_base::out);
		}

		ostream::~ostream() noexcept
		{
		}

		ostream&
		ostream::write(const char* str, std::streamsize cnt) noexcept
		{
			assert(cnt != 0);

			try
			{
				ios_base::iostate state = ios_base::goodbit;

				const osentry ok(this);

				if (ok)
				{
					_count = this->rdbuf()->write(str, cnt);
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

		ostream&
		ostream::write(const char* str, streamsize size, streamsize cnt) noexcept
		{
			return this->write(str, size * cnt);
		}

		ostream&
		ostream::flush() noexcept
		{
			if (this->rdbuf() != 0)
			{
				const osentry ok(this);
				if (ok)
				{
					if (ok && this->rdbuf()->flush() == -1)
						this->setstate(ios_base::badbit, true);
				}
			}

			return (*this);
		}

		ostream&
		ostream::seekg(ios_base::off_type pos) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, ios_base::out) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		ostream&
		ostream::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!this->fail() && (ios_base::off_type)this->rdbuf()->seekg(pos, dir) == ios_base::_BADOFF)
					this->setstate(ios_base::failbit);
			}

			return (*this);
		}

		streamsize
		ostream::gcount() const noexcept
		{
			return _count;
		}

		ios_base::pos_type
		ostream::tellg() noexcept
		{
			const osentry ok(this);

			if (!this->fail())
				return (this->rdbuf()->tellg());
			else
				return (ios_base::pos_type(ios_base::_BADOFF));
		}

		ostream&
		ostream::operator << (const char* str) noexcept
		{
			return this->write(str, (std::streamsize)std::strlen(str));
		}

		ostream&
		ostream::operator << (const std::string& str) noexcept
		{
			return this->write(str.c_str(), (std::streamsize)str.size());
		}

		ostream&
		ostream::operator << (char value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (unsigned char value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (short value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (long long value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (unsigned short value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (int value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (unsigned int value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (unsigned long value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (unsigned long long value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::itoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (float value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::ftoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (double value) noexcept
		{
			const osentry _Ok(this);
			if (_Ok)
			{
				char buf[PATHLIMIT];

				if (this->flags() & ios_base::dec)
				{
					auto written = runtime::string::dtoa10(buf, PATHLIMIT, value);
					if (written > 0)
					{
						return this->write(buf, written);
					}
				}
			}

			this->setstate(ios_base::failbit);
			return *this;
		}

		ostream&
		ostream::operator << (ios_base& (*function)(ios_base&)) noexcept
		{    // call ios_base manipulator
			assert(function);
			(*function)(*(ios_base *)this);
			return (*this);
		}

		ostream&
		ostream::operator << (ostream& (*function)(ostream&)) noexcept
		{
			assert(function);
			(*function)(*this);
			return (*this);
		}
	}
}