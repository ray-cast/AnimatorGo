#ifndef OCTOON_IOSTREAM_H_
#define OCTOON_IOSTREAM_H_

#include <octoon/io/istream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT iostream : public istream, public ostream
		{
		public:
			iostream(stream_buf* buf) noexcept;
			virtual ~iostream() noexcept;

			iostream& read(char* str, std::streamsize cnt) noexcept;
			iostream& read(char* str, streamsize size, streamsize cnt) noexcept;

			iostream& write(const char* str, std::streamsize cnt) noexcept;
			iostream& write(const char* str, streamsize size, streamsize cnt) noexcept;

			iostream& seekg(ios_base::off_type pos) noexcept;
			iostream& seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;

			iostream& flush() noexcept;

			streamsize size() noexcept;

			streamsize gcount() const noexcept;

			streampos tellg() noexcept;

		protected:
			class iosentry final
			{
			public:
				iosentry(iostream* _istr);
				~iosentry() noexcept;

				operator bool() const noexcept;

			private:
				iosentry(const iosentry&) = delete;
				const iosentry& operator=(const iosentry&) = delete;

			private:
				bool _ok;
				iostream* _myIostr;
			};

		private:
			iostream & operator=(const iostream&) noexcept = delete;
			iostream(const iostream&) noexcept = delete;

		private:
			streamsize _count;
		};
	}
}

#endif