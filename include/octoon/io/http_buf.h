#ifndef OCTOON_HTTP_BUF_H_
#define OCTOON_HTTP_BUF_H_

#include <octoon/io/stream_buf.h>
#include <octoon/io/file.h>
#include <octoon/runtime/json.h>
#include <vector>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT httpbuf final : public stream_buf
		{
		public:
			httpbuf() noexcept;
			~httpbuf() noexcept;

			bool is_open() const noexcept;

			bool get(const char* url, std::uint32_t timeout = 0, const std::string & ssl="") noexcept;
			bool get(const std::string& url, std::uint32_t timeout = 0, const std::string & ssl ="") noexcept;

			bool post(const char* url, const std::string& data, const std::string& headers = "content-type:application/json", std::uint32_t timeout = 0, const std::string& ssl = "") noexcept;
			bool post(const std::string& url, const std::string& data, const std::string& headers = "content-type:application/json", std::uint32_t timeout = 0, const std::string & ssl = "") noexcept;

			bool close() noexcept;

			runtime::json json() const noexcept;

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
