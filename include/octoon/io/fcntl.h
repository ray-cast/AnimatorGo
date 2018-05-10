#ifndef OCTOON_FCNTL_H_
#define OCTOON_FCNTL_H_

#include <string>
#include <sys/stat.h>

#include <octoon/runtime/platform.h>

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
#	include <sys/fcntl.h>
#	include <unistd.h>
#	include <dirent.h>
#	include <string.h>
#	include <strings.h>
#endif

namespace octoon
{
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

	namespace io
	{
		namespace fcntl
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
}

#endif