#include <octoon/io/file.h>
#include <octoon/io/fcntl.h>

#include <limits>
#include <codecvt>
#include <sys/stat.h>

#if defined(_WINDOWS_)
#   include <dos.h>
#   include <io.h>
#	include <fcntl.h>
#elif defined(__LINUX__) || defined(__ANDROID__) || defined(__APPLE__)
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <sys/param.h>
#	include <sys/fcntl.h>
#	include <unistd.h>
#	include <dirent.h>
#	include <string.h>
#	include <strings.h>
#else
#	include <sys/stat.h>
#	include <sys/types.h>
#	include <sys/param.h>
#	include <fcntl.h>
#	include <unistd.h>
#	include <dirent.h>
#	include <string.h>
#	include <strings.h>
#endif

#ifndef _IOMYBUF
#    define _IOMYBUF 0x0008
#endif

#ifndef _IOEOF
#   define _IOEOF 0x0010
#endif

#ifndef _IOERR
#   define _IOERR 0x0020
#endif

#if defined(__WINDOWS__)
#	define SEPARATOR '\\'
#	define SEPARATOR_STRING "\\"
#else
#	define SEPARATOR '/'
#	define SEPARATOR_STRING "/"
#endif

#if defined(__WINDOWS__) || defined(__MINGW64__)
#   define  POSIX(func) _ ## func
#else
#   define  POSIX(func) func
#endif

#define OCTOON_POSIX_ACCESS   POSIX(access)
#define OCTOON_POSIX_READ     POSIX(read)
#define OCTOON_POSIX_WRITE    POSIX(write)
#define OCTOON_POSIX_CLOSE    POSIX(close)
#if defined(__WINDOWS__)
#    define OCTOON_POSIX_WACCESS  POSIX(waccess)
#endif

#if __WINDOWS__
#   define OCTOON_POSIX_FLUSH    POSIX(commit)
#endif

#if _HUGE_FILES_
#   define OCTOON_POSIX_STAT   POSIX(stat64)
#   define OCTOON_POSIX_OPEN   POSIX(open64)
#   define OCTOON_POSIX_WOPEN  POSIX(wopen64)
#   define OCTOON_POSIX_SEEK   POSIX(lseeki64)
#    if defined(__WINDOWS__)
#        define OCTOON_POSIX_WOPEN    POSIX(wopen)
#        define OCTOON_POSIX_WSTAT    POSIX(wstat64)
#        define OCTOON_POSIX_TELL     POSIX(telli64)
#    endif
#else
#   define OCTOON_POSIX_STAT   POSIX(stat64)
#   define OCTOON_POSIX_OPEN   POSIX(open)
#   define OCTOON_POSIX_LSEEK  POSIX(lseek)
#    if defined(__WINDOWS__)
#        define OCTOON_POSIX_WOPEN    POSIX(wopen)
#        define OCTOON_POSIX_WSTAT    POSIX(wstat64)
#        define OCTOON_POSIX_TELL     POSIX(tell)
#    endif
#endif

namespace octoon
{
	namespace io
	{
		constexpr int PATHLIMIT = 4096;

		enum POSIXPERMISSIONS
		{
			PP_IXOTH = 1,
			PP_IWOTH = 2,
			PP_IROTH = 4,

			PP_IXGRP = 10,
			PP_IWGRP = 20,
			PP_IRGRP = 40,

			PP_IXUSR = 100,
			PP_IWUSR = 200,
			PP_IRUSR = 400,

			PP_DEFAULT = PP_IRUSR | PP_IWUSR | PP_IRGRP | PP_IWGRP | PP_IROTH | PP_IWOTH
		};

		inline int access(const char* path, int mode)
		{
			return ::OCTOON_POSIX_ACCESS(path, mode);
		}

		inline int access(const std::string& path, int mode)
		{
			return ::OCTOON_POSIX_ACCESS(path.data(), mode);
		}

		inline int access(const wchar_t* path, int mode)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WACCESS(path, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, path, PATHLIMIT) == (std::size_t) - 1)
				return EOF;

			return ::OCTOON_POSIX_ACCESS(fn, mode);
#else
			return 0;
#endif
		}

		inline int access(const std::wstring& path, int mode)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WACCESS(path.c_str(), mode);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, path.c_str(), PATHLIMIT) == (std::size_t) - 1)
				return EOF;

			return ::OCTOON_POSIX_ACCESS(fn, mode);
#else
			return 0;
#endif
		}

		inline int stat64(const char* filename, struct OCTOON_POSIX_STAT* stat)
		{
			return ::OCTOON_POSIX_STAT(filename, stat);
		}

		inline int stat64(const std::string& filename, struct OCTOON_POSIX_STAT* stat)
		{
			return ::OCTOON_POSIX_STAT(filename.data(), stat);
		}

		inline int stat64(const wchar_t* filename, struct OCTOON_POSIX_STAT* stat)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WSTAT(filename, stat);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, filename, PATHLIMIT) == (std::size_t) - 1)
				return EOF;
			return ::OCTOON_POSIX_STAT(fn, stat);
#else
			return EOF;
#endif
		}

		inline int stat64(const std::wstring& filename, struct OCTOON_POSIX_STAT* stat)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WSTAT(filename.data(), stat);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, filename.c_str(), PATHLIMIT) == (std::size_t) - 1)
				return EOF;
			return ::OCTOON_POSIX_STAT(fn, stat);
#else
			return EOF;
#endif
		}

		inline int open(const char* filename, int flag, int mode)
		{
			return ::OCTOON_POSIX_OPEN(filename, flag, mode);
		}

		inline int open(const std::string& filename, int flag, int mode)
		{
			return ::OCTOON_POSIX_OPEN(filename.data(), flag, mode);
		}

		inline int open(const wchar_t* filename, int flag, int mode)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WOPEN(filename, flag, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, filename, PATHLIMIT) == (std::size_t) - 1)
				return EOF;

			return ::OCTOON_POSIX_OPEN(fn, flag, mode);
#else
			return EOF;
#endif
		}

		inline int open(const std::wstring& filename, int flag, int mode)
		{
#if defined(__WINDOWS__)
			return ::OCTOON_POSIX_WOPEN(filename.c_str(), flag, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
			char fn[PATHLIMIT];
			if (::wcstombs(fn, filename.c_str(), PATHLIMIT) == (std::size_t) - 1)
				return EOF;

			return ::OCTOON_POSIX_OPEN(fn, flag, mode);
#else
			return EOF;
#endif
		}

		inline long long seek(int fd, long offset, int origin)
		{
			return ::OCTOON_POSIX_LSEEK(fd, offset, origin);
		}

#if __WINDOWS__

		inline long long tell(int fd)
		{
			return ::OCTOON_POSIX_TELL(fd);
		}

#endif

		inline int read(int fd, void* buf, unsigned int cnt)
		{
			return ::OCTOON_POSIX_READ(fd, buf, cnt);
		}

		inline int write(int fd, const void* buf, unsigned int cnt)
		{
			return ::OCTOON_POSIX_WRITE(fd, buf, cnt);
		}

#if __WINDOWS__

		inline int flush(int fd)
		{
			return ::OCTOON_POSIX_FLUSH(fd);
		}

#endif

		inline int close(int fd)
		{
			return ::OCTOON_POSIX_CLOSE(fd);
		}

		inline bool mkdir(const char* path)
		{
			char name[PATHLIMIT];
			strcpy(name, path);
			std::size_t len = strlen(path);

			if (name[len - 1] != '/' && name[len - 1] != '\\')
			{
				strcat(name, "/");
				len += 1;
			}

			for (std::size_t i = 1; i < len; i++)
			{
				if (name[i] != '/' && name[i] != '\\')
					continue;

				name[i] = 0;

				if (access(name, 0) != 0)
				{
#if __WINDOWS__
					if (!CreateDirectory(name, 0))
						return false;
#elif __LINUX__
					if (::mkdir(name, S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO) == -1)
						return false;
#endif
				}

#if __WINDOWS__
				name[i] = '\\';
#else
				name[i] = '/';
#endif
			}

			return true;
		}

#if __WINDOWS__
		inline void splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
		{
			::_splitpath(path, drive, dir, fname, ext);
		}

		inline void splitpath(const char *path, char *drive, std::size_t drive_size, char *dir, std::size_t dir_size, char *fname, std::size_t fname_size, char *ext, std::size_t ext_size)
		{
			::_splitpath_s(path, drive, drive_size, dir, dir_size, fname, fname_size, ext, ext_size);
		}
#else
		inline void _split_whole_name(const char *whole_name, char *fname, char *ext)
		{
			const char *p_ext;

			p_ext = rindex(whole_name, '.');
			if (NULL != p_ext)
			{
				strcpy(ext, p_ext);
				snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
			}
			else
			{
				ext[0] = '\0';
				strcpy(fname, whole_name);
			}
		}

		inline void splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
		{
			const char *p_whole_name;

			drive[0] = '\0';
			if (NULL == path)
			{
				dir[0] = '\0';
				fname[0] = '\0';
				ext[0] = '\0';
				return;
			}

			if ('/' == path[strlen(path)])
			{
				strcpy(dir, path);
				fname[0] = '\0';
				ext[0] = '\0';
				return;
			}

			p_whole_name = rindex(path, '/');
			if (NULL != p_whole_name)
			{
				p_whole_name++;
				_split_whole_name(p_whole_name, fname, ext);

				snprintf(dir, p_whole_name - path, "%s", path);
			}
			else
			{
				_split_whole_name(path, fname, ext);
				dir[0] = '\0';
			}
		}
#endif

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

		_Iobuf* fopen(const char* filepath, ios_base::openmode mode) noexcept
		{
		#if defined(__WINDOWS__)
			int flags = O_BINARY;
		#else
			int flags = 0;
		#endif

			if (mode & ios_base::in && mode & ios_base::out)
				flags |= O_RDWR | O_CREAT;
			else if (mode & ios_base::in)
				flags |= O_RDONLY;
			else if (mode & ios_base::out)
				flags |= O_WRONLY | O_TRUNC;

			if (mode & ios_base::app)     flags |= O_APPEND;
			if (mode & ios_base::trunc)   flags |= O_TRUNC;

			int access = io::PP_DEFAULT;

		#if defined(__WINDOWS__)
			access |= io::PP_IRUSR | io::PP_IWUSR;
		#endif

			std::wstring u16_conv = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(filepath);
			return fmalloc(io::open(u16_conv.c_str(), flags, access));
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
				flags |= O_RDWR | O_CREAT;
			else if (mode & ios_base::in)
				flags |= O_RDONLY;
			else if (mode & ios_base::out)
				flags |= O_WRONLY | O_TRUNC;

			if (mode & ios_base::app)     flags |= O_APPEND;
			if (mode & ios_base::trunc)   flags |= O_TRUNC;

			int access = io::PP_DEFAULT;

		#if defined(__WINDOWS__)
			access |= io::PP_IRUSR | io::PP_IWUSR;
		#endif

			return fmalloc(io::open(filename, flags, access));
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

				int ret = close(stream->_file);

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
						int result = read(stream->_file, buf, count);
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
				return (streamsize)read(stream->_file, buf, (unsigned int)size);
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
						int result = write(stream->_file, buf, count);
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
				return (streamsize)write(stream->_file, buf, (unsigned int)size);
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
			return tell(stream->_file);
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

					io::seek(stream->_file, count, seek);
				}

				return ftell(stream);
			}
			else
			{
				return io::seek(stream->_file, (long)_off, seek);
			}
		}

		int faccess(const char* path, ios_base::open_mode mode) noexcept
		{
			int flags = 0;
			if (mode != ios_base::out)
				flags |= 2;
			if (mode != ios_base::in)
				flags |= 4;
			return io::access(path, flags);
		}

		int faccess(const wchar_t* path, ios_base::open_mode mode) noexcept
		{
			int flags = 0;
			if (mode != ios_base::out)
				flags |= 2;
			if (mode != ios_base::in)
				flags |= 4;
			return io::access(path, flags);
		}

		File::File() noexcept
			:stream_(nullptr)
		{
		}

		File::File(const char* filename, ios_base::openmode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}

		File::File(const char* filename, ios_base::open_mode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}

		File::File(const wchar_t* filename, ios_base::openmode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}
		
		File::File(const wchar_t* filename, ios_base::open_mode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}
		
		File::File(const std::string& filename, ios_base::openmode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}
		
		File::File(const std::string& filename, ios_base::open_mode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}
		
		File::File(const std::wstring& filename, ios_base::openmode mode) noexcept
			: File()
		{
			this->open(filename, mode);
		}

		File::File(const std::wstring& filename, ios_base::open_mode mode) noexcept
			: File()
		{
			this->open(filename, mode);
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

#undef OCTOON_POSIX_ACCESS
#undef OCTOON_POSIX_CLOSE
#undef OCTOON_POSIX_FLUSH
#undef OCTOON_POSIX_WRITE
#undef OCTOON_POSIX_OPEN
#undef OCTOON_POSIX_STAT
#undef OCTOON_POSIX_READ
#undef OCTOON_POSIX_TELL
#undef OCTOON_POSIX_SEEK
#undef OCTOON_POSIX_LSEEK
#undef OCTOON_POSIX_WOPEN
#undef OCTOON_POSIX_WSTAT
#undef OCTOON_POSIX_WACCESS
#undef SEPARATOR
#undef SEPARATOR_STRING