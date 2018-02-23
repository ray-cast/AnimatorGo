#include <octoon/io/file.h>
#include <octoon/io/fcntl.h>

#include <limits>

#ifndef _IOMYBUF
#    define _IOMYBUF 0x0008
#endif

#ifndef _IOEOF
#   define _IOEOF 0x0010
#endif

#ifndef _IOERR
#   define _IOERR 0x0020
#endif

namespace octoon
{
	namespace io
	{
		struct _Iobuf
		{
			int _file;
			int _flag;
			char *_ptr;
			char *_base;
			int   _charbuf;
			int   _bufsiz;
			streamsize   _cnt;
		};

		enum { _INTERNAL_BUFSIZ = 4096 };

		_Iobuf* fopen(const char* filename, ios_base::openmode mode) noexcept;
		_Iobuf* fopen(const char* filename, ios_base::open_mode mode) noexcept;
		_Iobuf* fopen(const wchar_t* filename, ios_base::openmode mode) noexcept;
		_Iobuf* fopen(const wchar_t* filename, ios_base::open_mode mode) noexcept;
		_Iobuf* fopen(const std::string& filename, ios_base::openmode mode) noexcept;
		_Iobuf* fopen(const std::string& filename, ios_base::open_mode mode) noexcept;
		_Iobuf* fopen(const std::wstring& filename, ios_base::openmode mode) noexcept;
		_Iobuf* fopen(const std::wstring& filename, ios_base::open_mode mode) noexcept;

		int fflush(_Iobuf* stream);
		int fclose(_Iobuf* stream) noexcept;

		streamsize fread(void* buf, streamsize size, _Iobuf* stream) noexcept;
		streamsize fwrite(const void* buffer, streamsize size, _Iobuf* stream) noexcept;

		int fgetc(_Iobuf* stream) noexcept;

		streamoff ftell(_Iobuf* stream) noexcept;
		streamoff fseek(_Iobuf* stream, streamoff _off, ios_base::seekdir seek) noexcept;
		streamoff fgetpos(_Iobuf* stream);
		streamoff fsetpos(_Iobuf* stream);

		int faccess(const char* path, ios_base::open_mode mode) noexcept;
		int faccess(const wchar_t* path, ios_base::open_mode mode) noexcept;

		_Iobuf* fmalloc(int file) noexcept
		{
			if (file != EOF)
			{
				_Iobuf* stream = (_Iobuf*)::malloc(sizeof(_Iobuf));
				if (stream != nullptr)
				{
					stream->_file = file;
					stream->_base = nullptr;
					stream->_ptr = nullptr;
					stream->_bufsiz = 0;
					stream->_flag = 0;

					return stream;
				}
			}

			return nullptr;
		}

		_Iobuf* fopen(const char* filename, ios_base::openmode mode) noexcept
		{
		#if defined(__WINDOWS__)
			int flags = O_BINARY;
		#else
			int flags = 0;
		#endif

			if (mode & ios_base::in && mode & ios_base::out)
				flags |= O_RDWR;
			else if (mode & ios_base::in)
				flags |= O_RDONLY;
			else if (mode & ios_base::out)
				flags |= O_WRONLY | O_CREAT | O_TRUNC;

			if (mode & ios_base::app)     flags |= O_APPEND;
			if (mode & ios_base::trunc)   flags |= O_TRUNC;

			int access = fcntl::PP_DEFAULT;

		#if defined(__WINDOWS__)
			access |= fcntl::PP_IRUSR | fcntl::PP_IWUSR;
		#endif

			return fmalloc(fcntl::open(filename, flags, access));
		}

		_Iobuf* fopen(const char* filename, ios_base::open_mode mode) noexcept
		{
			return fopen(filename, (ios_base::openmode)mode);
		}

		_Iobuf* fopen(const wchar_t* filename, ios_base::openmode mode) noexcept
		{
		#if defined(__WINDOWS__)
			int flags = O_BINARY;
		#else
			int flags = 0;
		#endif

			if (mode & ios_base::in && mode & ios_base::out)
				flags |= O_RDWR;
			else if (mode & ios_base::in)
				flags |= O_RDONLY;
			else if (mode & ios_base::out)
				flags |= O_WRONLY | O_CREAT;

			if (mode & ios_base::app)     flags |= O_APPEND;
			if (mode & ios_base::trunc)   flags |= O_TRUNC;

			int access = fcntl::PP_DEFAULT;

		#if defined(__WINDOWS__)
			access |= fcntl::PP_IRUSR | fcntl::PP_IWUSR;
		#endif

			return fmalloc(fcntl::open(filename, flags, access));
		}

		_Iobuf* fopen(const wchar_t* filename, ios_base::open_mode mode) noexcept
		{
			return fopen(filename, (ios_base::openmode)mode);
		}

		_Iobuf* fopen(const std::string& filename, ios_base::openmode mode) noexcept
		{
			return fopen(filename.c_str(), mode);
		}

		_Iobuf* fopen(const std::string& filename, ios_base::open_mode mode) noexcept
		{
			return fopen(filename.c_str(), (ios_base::openmode)mode);
		}

		_Iobuf* fopen(const std::wstring& filename, ios_base::openmode mode) noexcept
		{
			return fopen(filename.c_str(), mode);
		}

		_Iobuf* fopen(const std::wstring& filename, ios_base::open_mode mode) noexcept
		{
			return fopen(filename.c_str(), (ios_base::openmode)mode);
		}

		int fclose(_Iobuf* stream) noexcept
		{
			if (stream)
			{
				if (stream->_base)
				{
					free(stream->_base);
				}

				int ret = fcntl::close(stream->_file);

				free(stream);

				return ret;
			}

			return 0;
		}

		streamsize fread(void* buf, streamsize size, _Iobuf* stream) noexcept
		{
			if (size > std::numeric_limits<unsigned int>::max())
			{
				streamsize out = 0;
				streamsize number = size;

				unsigned int count = std::numeric_limits<unsigned int>::max();

				for (;;)
				{
					if (number > count)
					{
						number -= count;
					}
					else
					{
						count = (unsigned int)number;
					}

					if (count > 0)
					{
						int result = fcntl::read(stream->_file, buf, count);
						if (result != 0)
						{
							out += result;
							buf = (char*)buf + result;

							if ((unsigned int)result != count)
							{
								break;
							}
						}
					}
				}

				return out;
			}
			else
			{
				return (streamsize)fcntl::read(stream->_file, buf, (unsigned int)size);
			}
		}

		streamsize fwrite(const void* buf, streamsize size, _Iobuf* stream) noexcept
		{
			if (size > std::numeric_limits<unsigned int>::max())
			{
				streamsize out = 0;
				streamsize number = size;

				unsigned int count = std::numeric_limits<unsigned int>::max();

				for (;;)
				{
					if (number > count)
					{
						number -= count;
					}
					else
					{
						count = (unsigned int)number;
					}

					if (count > 0)
					{
						int result = fcntl::write(stream->_file, buf, count);
						if (result != 0)
						{
							out += result;
							buf = (char*)buf + result;

							if ((unsigned int)result != count)
							{
								break;
							}
						}
					}
				}

				return out;
			}
			else
			{
				return (streamsize)fcntl::write(stream->_file, buf, (unsigned int)size);
			}
		}

		int fgetc(_Iobuf* stream) noexcept
		{
			assert(stream);

			if (!stream->_base)
			{
				stream->_base = (char*)malloc(_INTERNAL_BUFSIZ);
			}

			if (stream->_base)
			{
				stream->_flag |= _IOMYBUF;
				stream->_bufsiz = _INTERNAL_BUFSIZ;
			}
			else
			{
				stream->_flag |= _IONBF;
				stream->_base = (char *)&(stream->_charbuf);
				stream->_bufsiz = 2;
			}

			stream->_ptr = stream->_base;
			stream->_cnt = fread(stream->_base, stream->_bufsiz, stream);

			if ((stream->_cnt == 0) || (stream->_cnt == -1))
			{
				stream->_flag |= stream->_cnt ? _IOERR : _IOEOF;
				stream->_cnt = 0;
				return EOF;
			}

		#ifndef _UNICODE
			stream->_cnt--;
			return (0xff & *stream->_ptr++);
		#else
			stream->_cnt -= sizeof(wchar_t);
			return (0xffff & *((wchar_t *)(*stream->_ptr++)));
		#endif
		}

		streamoff ftell(_Iobuf* stream) noexcept
		{
		#if defined(__WINDOWS__) || defined(__DOS__)
			return fcntl::tell(stream->_file);
		#else
			return fseek(stream, 0, ios_base::cur);
		#endif
		}

		streamoff fseek(_Iobuf* stream, streamoff _off, ios_base::seekdir seek) noexcept
		{
			// Just to check whether our enum maps one to one with the CRT constants
			static_assert(ios_base::beg == SEEK_SET &&
				ios_base::cur == SEEK_CUR &&
				ios_base::end == SEEK_END,
				"The seekdir does not match.");

			assert(seek == SEEK_SET || seek == SEEK_CUR || seek == SEEK_END);

			if (_off > std::numeric_limits<long>::max())
			{
				streamoff number = _off;

				long count = std::numeric_limits<long>::max();

				for (;;)
				{
					if (number > count)
					{
						number -= count;
					}
					else
					{
						count = (long)number;
					}

					if (count == 0)
					{
						break;
					}

					fcntl::seek(stream->_file, count, seek);
				}

				return ftell(stream);
			}
			else
			{
				return fcntl::seek(stream->_file, (long)_off, seek);
			}
		}

		int faccess(const char* path, ios_base::open_mode mode) noexcept
		{
			int flags = 0;
			if (mode != ios_base::out)
				flags |= 2;
			if (mode != ios_base::in)
				flags |= 4;
			return fcntl::access(path, flags);
		}

		int faccess(const wchar_t* path, ios_base::open_mode mode) noexcept
		{
			int flags = 0;
			if (mode != ios_base::out)
				flags |= 2;
			if (mode != ios_base::in)
				flags |= 4;
			return fcntl::access(path, flags);
		}

		File::File() noexcept
			:stream_(nullptr)
		{
		}

		File::~File() noexcept
		{
			this->close();
		}

		bool
		File::is_open() const noexcept
		{
			return stream_ != nullptr;
		}

		File*
		File::open(const char* filename, ios_base::openmode mode) noexcept
		{
			if (stream_)
				fclose(stream_);

			stream_ = fopen(filename, mode);
			if (stream_)
				return this;
			return nullptr;
		}

		File*
		File::open(const wchar_t* filename, ios_base::openmode mode) noexcept
		{
			if (stream_)
				fclose(stream_);

			stream_ = fopen(filename, mode);
			if (stream_)
				return this;

			return nullptr;
		}

		File*
		File::open(const char* filename, ios_base::open_mode mode) noexcept
		{
			return this->open(filename, (ios_base::openmode)mode);
		}

		File*
		File::open(const wchar_t* filename, ios_base::open_mode mode) noexcept
		{
			return this->open(filename, (ios_base::openmode)mode);
		}

		File*
		File::open(const std::string& filename, ios_base::openmode mode) noexcept
		{
			return this->open(filename.c_str(), mode);
		}

		File*
		File::open(const std::string& filename, ios_base::open_mode mode) noexcept
		{
			return this->open(filename.c_str(), (ios_base::openmode)mode);
		}

		File*
		File::open(const std::wstring& filename, ios_base::openmode mode) noexcept
		{
			return this->open(filename.c_str(), mode);
		}

		File*
		File::open(const std::wstring& filename, ios_base::open_mode mode) noexcept
		{
			return this->open(filename.c_str(), (ios_base::openmode)mode);
		}

		int
		File::getc() noexcept
		{
			return fgetc(stream_);
		}

		streamoff
		File::seek(streamoff _off, ios_base::seekdir seek) noexcept
		{
			return fseek(stream_, _off, seek);
		}

		bool
		File::close() noexcept
		{
			if (stream_)
			{
				fclose(stream_);
				stream_ = nullptr;
				return true;
			}

			return false;
		}

		streamoff
		File::tell() const noexcept
		{
			return ftell(stream_);
		}

		streamsize
		File::size() const noexcept
		{
			std::size_t off = this->tell();
			fseek(stream_, 0, ios_base::end);
			std::size_t size = this->tell();
			fseek(stream_, off, ios_base::beg);
			return size;
		}

		streamsize
		File::read(void* buf, streamsize size) noexcept
		{
			return fread(buf, size, stream_);
		}

		streamsize
		File::write(const void* buf, streamsize size) noexcept
		{
			return fwrite(buf, size, stream_);
		}

		int
		File::flag() noexcept
		{
			return stream_->_flag;
		}

		char*
		File::ptr()  noexcept
		{
			return stream_->_ptr;
		}

		char*
		File::base() noexcept
		{
			return stream_->_base;
		}

		streamsize
		File::cnt() noexcept
		{
			return stream_->_cnt;
		}

		void
		File::swap(File& other) noexcept
		{
			std::swap(stream_, other.stream_);
		}
	}
}