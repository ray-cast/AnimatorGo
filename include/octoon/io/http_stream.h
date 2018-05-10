#ifndef OCTOON_IO_HTTP_STREAM_H_
#define OCTOON_IO_HTTP_STREAM_H_

#include <octoon/io/http_buf.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT http_reader final : public istream
		{
		public:
			http_reader() noexcept;
			http_reader(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			http_reader(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			~http_reader() noexcept;

			http_reader& open(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			http_reader& open(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;

			http_reader& close() noexcept;

			bool is_open() const noexcept;

		private:
			http_reader(const http_reader&) = delete;
			http_reader& operator=(const http_reader&) = delete;

		private:
			httpbuf http_;
		};

		class OCTOON_EXPORT http_write final : public ostream
		{
		public:
			http_write() noexcept;
			http_write(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			http_write(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			~http_write() noexcept;

			http_write& open(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			http_write& open(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;

			http_write& close() noexcept;

			bool is_open() const noexcept;

		private:
			http_write(const http_write&) = delete;
			http_write& operator=(const http_write&) = delete;

		private:
			httpbuf http_;
		};

		class OCTOON_EXPORT http_stream final : public iostream
		{
		public:
			http_stream() noexcept;
			http_stream(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			http_stream(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			~http_stream() noexcept;

			http_stream& open(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			http_stream& open(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;

			http_stream& close() noexcept;

			bool is_open() const noexcept;

		private:
			http_stream(const http_stream&) = delete;
			http_stream& operator=(const http_stream&) = delete;

		private:
			httpbuf http_;
		};
	}
}

#endif
