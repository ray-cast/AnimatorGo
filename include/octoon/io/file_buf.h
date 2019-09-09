#ifndef OCTOON_FILE_BUF_H_
#define OCTOON_FILE_BUF_H_

#include <octoon/io/stream_buf.h>
#include <octoon/io/file.h>

namespace octoon
{
	namespace io
	{
		class filebuf final : public stream_buf
		{
		public:
			filebuf() noexcept;
			filebuf(filebuf&& move) noexcept;
			~filebuf() noexcept;

			bool is_open() const noexcept;

			bool open(const char* filename, ios_base::openmode mode) noexcept;
			bool open(const wchar_t* filename, ios_base::openmode mode) noexcept;
			bool open(const std::string& filename, ios_base::openmode mode) noexcept;
			bool open(const std::wstring& filename, ios_base::openmode mode) noexcept;

			bool close() noexcept;

			streamsize read(char* str, std::streamsize cnt) noexcept;
			streamsize write(const char* str, std::streamsize cnt) noexcept;

			streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
			streamoff tellg() noexcept;

			streamsize size() const noexcept;

			int flush() noexcept;

		private:
			filebuf(const filebuf&) = delete;
			filebuf& operator=(const filebuf&) = delete;

		private:
			File _file;
		};
	}
}

#endif
