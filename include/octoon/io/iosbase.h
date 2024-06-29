#ifndef OCTOON_IOSBASE_H_
#define OCTOON_IOSBASE_H_

#include <iosfwd>
#include <ios>
#include <cstdio>

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace io
	{
		using streamoff = std::streamoff;
		using streampos = std::streampos ;
		using streamsize = std::streamsize;

		class OCTOON_EXPORT ios_base
		{
		public:
			typedef int iostate;
			typedef int openmode;
			typedef int seekdir;
			typedef int fmtflags;

			typedef unsigned int io_state, open_mode, seek_dir, file_type;

			typedef int int_type;
			typedef int pos_type;
			typedef int off_type;

			enum _FmtFlags { _Fmtmask = 0xffff, _Fmtzero = 0 };
			static const _FmtFlags skipws = (_FmtFlags)std::ios_base::skipws;
			static const _FmtFlags unitbuf = (_FmtFlags)std::ios_base::unitbuf;
			static const _FmtFlags uppercase = (_FmtFlags)std::ios_base::uppercase;
			static const _FmtFlags showbase = (_FmtFlags)std::ios_base::showbase;
			static const _FmtFlags showpoint = (_FmtFlags)std::ios_base::showpoint;
			static const _FmtFlags showpos = (_FmtFlags)std::ios_base::showpos;
			static const _FmtFlags left = (_FmtFlags)std::ios_base::left;
			static const _FmtFlags right = (_FmtFlags)std::ios_base::right;
			static const _FmtFlags internal = (_FmtFlags)std::ios_base::internal;
			static const _FmtFlags dec = (_FmtFlags)std::ios_base::dec;
			static const _FmtFlags oct = (_FmtFlags)std::ios_base::oct;
			static const _FmtFlags hex = (_FmtFlags)std::ios_base::hex;
			static const _FmtFlags scientific = (_FmtFlags)std::ios_base::scientific;
			static const _FmtFlags fixed = (_FmtFlags)std::ios_base::fixed;
			static const _FmtFlags boolalpha = (_FmtFlags)std::ios_base::boolalpha;

			enum _Iostate { _Statmask = 0x17 };
			static const _Iostate goodbit = (_Iostate)std::ios_base::goodbit;
			static const _Iostate eofbit = (_Iostate)std::ios_base::eofbit;
			static const _Iostate failbit = (_Iostate)std::ios_base::failbit;
			static const _Iostate badbit = (_Iostate)std::ios_base::badbit;

			enum _Openmode { _Openmask = 0xff };
			static const _Openmode in = (_Openmode)std::ios_base::in;
			static const _Openmode out = (_Openmode)std::ios_base::out;
			static const _Openmode ate = (_Openmode)std::ios_base::ate;
			static const _Openmode app = (_Openmode)std::ios_base::app;
			static const _Openmode trunc = (_Openmode)std::ios_base::trunc;
			static const _Openmode binary = (_Openmode)std::ios_base::binary;

			enum _Seekdir { _Seekmask = 0x3 };
			static const _Seekdir beg = (_Seekdir)std::ios_base::beg;
			static const _Seekdir cur = (_Seekdir)std::ios_base::cur;
			static const _Seekdir end = (_Seekdir)std::ios_base::end;

			/*
			* File system file types supported by `octoon-io`. This type is used to ensure
			* all FS operations are adopted as expected. `NA` stands for 'not available',
			* meaning that the file was not found or of a type out of capability of
			* `octoon-io`.
			*/
			enum _FileType { _Filemask = 0x2 };
			static const _FileType none = (_FileType)0x0;
			static const _FileType file = (_FileType)0x1;
			static const _FileType directory = (_FileType)0x2;

			enum { _Openprot = 0x40 };

			static const streamsize PATHLIMIT = 4096;
			static const streamoff _BADOFF = (streamoff)-1;

		public:
			ios_base() noexcept;
			~ios_base() noexcept;

			bool good() const noexcept;
			bool eof() const noexcept;
			bool fail() const noexcept;
			bool bad() const noexcept;

			ios_base::iostate rdstate() const noexcept;

			void exceptions(ios_base::iostate _new) noexcept;
			ios_base::iostate exceptions() const noexcept;

			ios_base::fmtflags flags() const noexcept;

			ios_base::open_mode mode() const noexcept;

			bool operator!() const noexcept;
			operator bool() const noexcept;

		protected:
			void _init(open_mode mode) noexcept;

			void clear(ios_base::io_state _state, open_mode mode) except;
			void clear(ios_base::iostate _state, open_mode mode) except;
			void clear(ios_base::iostate _state, open_mode mode, bool _reraise) except;

			void setstate(ios_base::iostate _state) noexcept;
			void setstate(ios_base::io_state _state) noexcept;
			void setstate(ios_base::iostate _state, bool _exreraise) noexcept;

			void setstate(ios_base::iostate _state, open_mode mode) noexcept;
			void setstate(ios_base::io_state _state, open_mode mode) noexcept;
			void setstate(ios_base::iostate _state, open_mode mode, bool _exreraise) noexcept;

		private:
			ios_base(const ios_base&) = delete;
			const ios_base& operator=(const ios_base&) = delete;

		private:
			fmtflags fmtfl_;
			iostate state_;
			iostate except_;
			open_mode mode_;
		};
	}
}

#endif