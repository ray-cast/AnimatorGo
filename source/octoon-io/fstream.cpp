// File: fstream.cpp
// Author: PENGUINLIONG
#include <octoon/io/fstream.h>

namespace octoon
{
	namespace io
	{
		ifstream::ifstream() noexcept
			: istream(&file_)
		{
		}

		ifstream::ifstream(const char* path, const ios_base::open_mode mode) noexcept
			: istream(&file_)
		{
			this->open(path, mode);
		}

		ifstream::ifstream(const wchar_t* path, const ios_base::open_mode mode) noexcept
			: istream(&file_)
		{
			this->open(path, mode);
		}

		ifstream::ifstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: istream(&file_)
		{
			this->open(path, mode);
		}

		ifstream::ifstream(const std::wstring& path, const ios_base::open_mode mode) noexcept
			: istream(&file_)
		{
			this->open(path, mode);
		}

		ifstream::~ifstream() noexcept
		{
			this->close();
		}

		bool
		ifstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		ifstream&
		ifstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ifstream&
		ifstream::open(const wchar_t* path, const ios_base::open_mode mode) noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ifstream&
		ifstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		ifstream&
		ifstream::open(const std::wstring& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		ifstream&
		ifstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!file_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		ofstream::ofstream() noexcept
			: ostream(&file_)
		{
		}

		ofstream::ofstream(const char* path, const ios_base::open_mode mode) noexcept
			: ostream(&file_)
		{
			this->open(path, mode);
		}

		ofstream::ofstream(const wchar_t* path, const ios_base::open_mode mode) noexcept
			: ostream(&file_)
		{
			this->open(path, mode);
		}

		ofstream::ofstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: ostream(&file_)
		{
			this->open(path, mode);
		}

		ofstream::ofstream(const std::wstring& path, const ios_base::open_mode mode) noexcept
			: ostream(&file_)
		{
			this->open(path, mode);
		}

		ofstream::~ofstream() noexcept
		{
			this->close();
		}

		bool
		ofstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		ofstream&
		ofstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ofstream&
		ofstream::open(const wchar_t* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		ofstream&
		ofstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		ofstream&
		ofstream::open(const std::wstring& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		ofstream&
		ofstream::close() noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!file_.close())
					this->setstate(failbit);
			}

			return (*this);
		}

		fstream::fstream() noexcept
			: iostream(&file_)
		{
		}

		fstream::fstream(const char* path, const ios_base::open_mode mode) noexcept
			: iostream(&file_)
		{
			this->open(path, mode);
		}

		fstream::fstream(const wchar_t* path, const ios_base::open_mode mode) noexcept
			: iostream(&file_)
		{
			this->open(path, mode);
		}

		fstream::fstream(const std::string& path, const ios_base::open_mode mode) noexcept
			: iostream(&file_)
		{
			this->open(path, mode);
		}

		fstream::fstream(const std::wstring& path, const ios_base::open_mode mode) noexcept
			: iostream(&file_)
		{
			this->open(path, mode);
		}

		fstream::~fstream() noexcept
		{
			this->close();
		}

		fstream&
		fstream::open(const char* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		fstream&
		fstream::open(const wchar_t* path, const ios_base::open_mode mode) noexcept
		{
			const osentry ok(this);
			if (ok)
			{
				if (!file_.open(path, mode))
					this->setstate(ios_base::failbit, mode);
				else
					this->clear(ios_base::goodbit, mode);
			}

			return (*this);
		}

		fstream&
		fstream::open(const std::string& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		fstream&
		fstream::open(const std::wstring& path, const ios_base::open_mode mode) noexcept
		{
			return this->open(path.c_str(), mode);
		}

		bool
		fstream::is_open() const noexcept
		{
			return this->rdbuf()->is_open();
		}

		fstream&
		fstream::close() noexcept
		{
			const isentry ok(this);
			if (ok)
			{
				if (!file_.close())
					this->setstate(failbit);
			}

			return (*this);
		}
	}
}