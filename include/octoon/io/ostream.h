#ifndef OCTOON_OSTREAM_H_
#define OCTOON_OSTREAM_H_

#include <octoon/io/stream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT ostream : public virtual stream
		{
		public:
			ostream(stream_buf* buf) noexcept;
			virtual ~ostream() noexcept;

			ostream& write(const char* str, std::streamsize cnt) noexcept;
			ostream& write(const char* str, std::streamsize size, std::streamsize cnt) noexcept;

			ostream& seekg(ios_base::off_type pos) noexcept;
			ostream& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

			ostream& flush() noexcept;

			streamsize gcount() const noexcept;

			ios_base::pos_type tellg() noexcept;

			ostream& operator << (const char* str) noexcept;
			ostream& operator << (const std::string& str) noexcept;
			ostream& operator << (char value) noexcept;
			ostream& operator << (short value) noexcept;
			ostream& operator << (int value) noexcept;
			ostream& operator << (long long value) noexcept;
			ostream& operator << (unsigned char value) noexcept;
			ostream& operator << (unsigned short value) noexcept;
			ostream& operator << (unsigned int value) noexcept;
			ostream& operator << (unsigned long value) noexcept;
			ostream& operator << (unsigned long long value) noexcept;
			ostream& operator << (float value) noexcept;
			ostream& operator << (double value) noexcept;
			ostream& operator << (ios_base& (*function)(ios_base&)) noexcept;
			ostream& operator << (ostream& (*function)(ostream&)) noexcept;

		protected:
			class osentry
			{
			public:
				osentry(ostream* _ostr);
				~osentry() noexcept;
				operator bool() const noexcept;

			private:
				osentry(const osentry&) noexcept = delete;
				const osentry& operator=(const osentry&) noexcept = delete;

			private:
				bool _ok;
				ostream* _my_ostr;
			};

		private:
			ostream & operator=(const ostream&) = delete;
			ostream(const ostream&) = delete;

		private:

			streamsize _count;
		};
	}
}

#endif
