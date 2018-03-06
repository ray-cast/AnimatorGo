#include <octoon/io/file_buf.h>

namespace octoon
{
	namespace io
	{
		filebuf::filebuf() noexcept
		{
		}

		filebuf::~filebuf() noexcept
		{
		}

		bool
		filebuf::is_open() const noexcept
		{
			return _file.is_open();
		}

		bool
		filebuf::open(const char* filename, ios_base::openmode mode) noexcept
		{
			return _file.open(filename, mode) ? true : false;
		}

		bool
		filebuf::open(const wchar_t* filename, ios_base::openmode mode) noexcept
		{
			return _file.open(filename, mode) ? true : false;
		}

		bool
		filebuf::open(const std::string& filename, ios_base::openmode mode) noexcept
		{
			return _file.open(filename, mode) ? true : false;
		}

		bool
		filebuf::open(const std::wstring& filename, ios_base::openmode mode) noexcept
		{
			return _file.open(filename, mode) ? true : false;
		}

		streamsize
		filebuf::read(char* str, std::streamsize cnt) noexcept
		{
			return _file.read(str, cnt);
		}

		streamsize
		filebuf::write(const char* str, std::streamsize cnt) noexcept
		{
			return _file.write(str, cnt);
		}

		streamoff
		filebuf::seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept
		{
			return _file.seek(pos, dir);
		}

		streamoff
		filebuf::tellg() noexcept
		{
			return _file.tell();
		}

		streamsize
		filebuf::size() const noexcept
		{
			return _file.size();
		}

		int
		filebuf::flush() noexcept
		{
			return 0;
		}

		bool
		filebuf::close() noexcept
		{
			return _file.close();
		}
	}
}