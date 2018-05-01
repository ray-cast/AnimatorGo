#ifndef OCTOON_STREAMBUF_H_
#define OCTOON_STREAMBUF_H_

#include <octoon/io/iosbase.h>

namespace octoon
{
	namespace io
	{
		/*
		* Base of all Octoon stream types. Provide fundamental I/O functionalities.
		*/
		class OCTOON_EXPORT stream_buf
		{
		public:
			stream_buf() noexcept = default;
			virtual ~stream_buf() noexcept = default;

			virtual streamsize read(char* str, std::streamsize cnt) noexcept = 0;
			virtual streamsize write(const char* str, std::streamsize cnt) noexcept = 0;

			virtual streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept = 0;
			virtual streamoff tellg() noexcept = 0;

			virtual streamsize size() const noexcept = 0;

			virtual bool is_open() const noexcept = 0;

			virtual int flush() noexcept = 0;

			virtual void lock() noexcept;
			virtual void unlock() noexcept;
		};
	}
}

#endif