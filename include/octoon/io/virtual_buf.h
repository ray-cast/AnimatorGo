#ifndef OCTOON_IO_VIRTUAL_BUF_H_
#define OCTOON_IO_VIRTUAL_BUF_H_

#include <memory>

#include <octoon/io/ori.h>
#include <octoon/io/stream_buf.h>

namespace octoon
{
	namespace io
	{
		class virtual_buf final : public stream_buf
		{
		public:
			virtual_buf() noexcept;
			~virtual_buf() noexcept;

			bool is_open() const noexcept;

			bool open(class IoServer* filesystem, const Orl& orl, const ios_base::open_mode mode) noexcept;
			void close() noexcept;

			streamsize read(char* str, std::streamsize cnt) noexcept;
			streamsize write(const char* str, std::streamsize cnt) noexcept;

			streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
			streamoff tellg() noexcept;

			streamsize size() const noexcept;

			int flush() noexcept;

		private:
			std::unique_ptr<stream_buf> buf_;
		};
	}
}

#endif