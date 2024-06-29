#ifndef OCTOON_FILE_H_
#define OCTOON_FILE_H_

#include <octoon/io/iosbase.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT File final
		{
		public:
			File() noexcept;
			File(File&& move) noexcept;
			File(const char* filename, ios_base::openmode mode) noexcept;
			File(const char* filename, ios_base::open_mode mode) noexcept;
			File(const char8_t* filename, ios_base::openmode mode) noexcept;
			File(const char8_t* filename, ios_base::open_mode mode) noexcept;
			File(const wchar_t* filename, ios_base::openmode mode) noexcept;
			File(const wchar_t* filename, ios_base::open_mode mode) noexcept;
			File(const std::string& filename, ios_base::openmode mode) noexcept;
			File(const std::string& filename, ios_base::open_mode mode) noexcept;
			File(const std::wstring& filename, ios_base::openmode mode) noexcept;
			File(const std::wstring& filename, ios_base::open_mode mode) noexcept;
			File(const std::u8string& filename, ios_base::openmode mode) noexcept;
			File(const std::u8string& filename, ios_base::open_mode mode) noexcept;
			~File() noexcept;

			File* open(const char* filename, ios_base::openmode mode) noexcept;
			File* open(const char* filename, ios_base::open_mode mode) noexcept;
			File* open(const char8_t* filename, ios_base::openmode mode) noexcept;
			File* open(const char8_t* filename, ios_base::open_mode mode) noexcept;
			File* open(const wchar_t* filename, ios_base::openmode mode) noexcept;
			File* open(const wchar_t* filename, ios_base::open_mode mode) noexcept;
			File* open(const std::string& filename, ios_base::openmode mode) noexcept;
			File* open(const std::string& filename, ios_base::open_mode mode) noexcept;
			File* open(const std::wstring& filename, ios_base::openmode mode) noexcept;
			File* open(const std::wstring& filename, ios_base::open_mode mode) noexcept;
			File* open(const std::u8string& filename, ios_base::openmode mode) noexcept;
			File* open(const std::u8string& filename, ios_base::open_mode mode) noexcept;
			
			bool is_open() const noexcept;
			bool close() noexcept;

			int getc() noexcept;

			streamoff seek(streamoff _off, ios_base::seekdir seek) noexcept;
			streamoff tell() const noexcept;

			streamsize size() const noexcept;

			streamsize read(void* buf, streamsize size) noexcept;
			streamsize write(const void* buf, streamsize size) noexcept;

			int flag() noexcept;
			char* ptr() noexcept;
			char* base() noexcept;
			streamsize cnt() noexcept;

			void swap(File& other) noexcept;

		private:
			File(const File&) = delete;
			File& operator=(const File&) = delete;

		private:
			struct _Iobuf* stream_;
		};
	}
}

#endif