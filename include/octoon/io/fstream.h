#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_

#include <octoon/io/file_buf.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT ifstream final : public istream
		{
		public:
			ifstream() noexcept;
			ifstream(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream(const char8_t* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream(const wchar_t* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream(const std::wstring& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream(const std::u8string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			~ifstream() noexcept;

			ifstream& open(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream& open(const char8_t* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream& open(const wchar_t* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream& open(const std::wstring& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ifstream& open(const std::u8string& path, const ios_base::open_mode mode = ios_base::in) noexcept;

			ifstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ifstream(const ifstream&) = delete;
			ifstream& operator=(const ifstream&) = delete;

		private:
			filebuf file_;
		};

		class OCTOON_EXPORT ofstream final : public ostream
		{
		public:
			ofstream() noexcept;
			ofstream(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream(const char8_t* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream(const wchar_t* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream(const std::wstring& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream(const std::u8string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			~ofstream() noexcept;

			ofstream& open(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream& open(const char8_t* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream& open(const wchar_t* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream& open(const std::wstring& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ofstream& open(const std::u8string& path, const ios_base::open_mode mode = ios_base::out) noexcept;

			ofstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ofstream(const ofstream&) = delete;
			ofstream& operator=(const ofstream&) = delete;

		private:
			filebuf file_;
		};

		class OCTOON_EXPORT fstream final : public iostream
		{
		public:
			fstream() noexcept;
			fstream(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream(const wchar_t* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream(const char8_t* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream(const std::wstring& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream(const std::u8string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			~fstream() noexcept;

			fstream& open(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream& open(const wchar_t* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream& open(const char8_t* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream& open(const std::wstring& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			fstream& open(const std::u8string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;

			fstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			fstream(const fstream&) = delete;
			fstream& operator=(const fstream&) = delete;

		private:
			filebuf file_;
		};
	}
}

#endif
