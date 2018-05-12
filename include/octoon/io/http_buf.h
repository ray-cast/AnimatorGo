#ifndef OCTOON_HTTP_BUF_H_
#define OCTOON_HTTP_BUF_H_

#include <octoon/io/stream_buf.h>
#include <octoon/io/file.h>
#include <vector>

namespace octoon
{
	namespace io
	{
		class httpbuf final : public stream_buf
		{
		public:
			httpbuf() noexcept;
			~httpbuf() noexcept;

			bool is_open() const noexcept;

			bool open(const char* url, ios_base::openmode mode) noexcept;
			bool open(const std::string& url, ios_base::openmode mode) noexcept;

			bool close() noexcept;

			streamsize read(char* str, std::streamsize cnt) noexcept;
			streamsize write(const char* str, std::streamsize cnt) noexcept;

			streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
			streamoff tellg() noexcept;

			streamsize size() const noexcept;

			int flush() noexcept;

		private:
			static std::size_t WriteMemoryCallback(void* contents, std::size_t size, std::size_t nmemb, void* userp) noexcept;

		private:
			void* curl_;
			std::streampos next_;
			std::vector<char> memory_;
		};
	}
}

#endif
