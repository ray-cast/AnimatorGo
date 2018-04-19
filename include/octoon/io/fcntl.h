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
#	include <strings.h>
#endif

namespace octoon
{
#if defined(__WINDOWS__) || defined(__MINGW64__)
#   define  POSIX(func) _ ## func
#else
#   define  POSIX(func) func
#endif

#define __access   POSIX(access)
#define __read     POSIX(read)
#define __write    POSIX(write)
#define __close    POSIX(close)
#if defined(__WINDOWS__)
#    define __waccess  POSIX(waccess)
#endif

#if __WINDOWS__
#   define __flush    POSIX(commit)
#endif

#if _HUGE_FILES_
#   define __stat   POSIX(stat64)
#   define __open   POSIX(open64)
#   define __wopen  POSIX(wopen64)
#   define __seek   POSIX(lseeki64)
#    if defined(__WINDOWS__)
#        define __wopen    POSIX(wopen)
#        define __wstat    POSIX(wstat64)
#        define __tell     POSIX(telli64)
#    endif
#else
#   define __stat   POSIX(stat64)
#   define __open   POSIX(open)
#   define __lseek  POSIX(lseek)
#    if defined(__WINDOWS__)
#        define __wopen    POSIX(wopen)
#        define __wstat    POSIX(wstat64)
#        define __tell     POSIX(tell)
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
				return ::__access(path, mode);
			}

			inline int access(const std::string& path, int mode)
			{
				return ::__access(path.data(), mode);
			}

			inline int access(const wchar_t* path, int mode)
			{
#if defined(__WINDOWS__)
				return ::__waccess(path, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, path, PATHLIMIT) == (std::size_t) - 1)
					return EOF;

				return ::__access(fn, mode);
#else
				return 0;
#endif
			}

			inline int access(const std::wstring& path, int mode)
			{
#if defined(__WINDOWS__)
				return ::__waccess(path.c_str(), mode);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, path.c_str(), PATHLIMIT) == (std::size_t) - 1)
					return EOF;

				return ::__access(fn, mode);
#else
				return 0;
#endif
			}

			inline int stat64(const char* filename, struct __stat* stat)
			{
				return ::__stat(filename, stat);
			}

			inline int stat64(const std::string& filename, struct __stat* stat)
			{
				return ::__stat(filename.data(), stat);
			}

			inline int stat64(const wchar_t* filename, struct __stat* stat)
			{
#if defined(__WINDOWS__)
				return ::__wstat(filename, stat);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, filename, PATHLIMIT) == (std::size_t) - 1)
					return EOF;
				return ::__stat(fn, stat);
#else
				return EOF;
#endif
			}

			inline int stat64(const std::wstring& filename, struct __stat* stat)
			{
#if defined(__WINDOWS__)
				return ::__wstat(filename.data(), stat);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, filename.c_str(), PATHLIMIT) == (std::size_t) - 1)
					return EOF;
				return ::__stat(fn, stat);
#else
				return EOF;
#endif
			}

			inline int open(const char* filename, int flag, int mode)
			{
				return ::__open(filename, flag, mode);
			}

			inline int open(const std::string& filename, int flag, int mode)
			{
				return ::__open(filename.data(), flag, mode);
			}

			inline int open(const wchar_t* filename, int flag, int mode)
			{
#if defined(__WINDOWS__)
				return ::__wopen(filename, flag, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, filename, PATHLIMIT) == (std::size_t) - 1)
					return EOF;

				return ::__open(fn, flag, mode);
#else
				return EOF;
#endif
			}

			inline int open(const std::wstring& filename, int flag, int mode)
			{
#if defined(__WINDOWS__)
				return ::__wopen(filename.c_str(), flag, mode);
#elif defined(__LINUX__) || defined(__APPLE__)
				char fn[PATHLIMIT];
				if (::wcstombs(fn, filename.c_str(), PATHLIMIT) == (std::size_t) - 1)
					return EOF;

				return ::__open(fn, flag, mode);
#else
				return EOF:
#endif
			}

			inline long long seek(int fd, long offset, int origin)
			{
				return ::__lseek(fd, offset, origin);
			}

#if __WINDOWS__

			inline long long tell(int fd)
			{
				return ::__tell(fd);
			}

#endif

			inline int read(int fd, void* buf, unsigned int cnt)
			{
				return ::__read(fd, buf, cnt);
			}

			inline int write(int fd, const void* buf, unsigned int cnt)
			{
				return ::__write(fd, buf, cnt);
			}

#if __WINDOWS__

			inline int flush(int fd)
			{
				return ::__flush(fd);
			}

#endif

			inline int close(int fd)
			{
				return ::__close(fd);
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
#undef __access
#undef __close
#undef __flush
#undef __write
#undef __open
#undef __stat
#undef __read
#undef __tell
#undef __seek
#undef __lseek
#undef __wopen
#undef __wstat
#undef __waccess
}

#endif