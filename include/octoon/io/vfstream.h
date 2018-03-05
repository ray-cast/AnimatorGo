// File: vfstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_VFSTREAM_H_
#define OCTOON_IO_VFSTREAM_H_

#include <octoon/io/ori.h>
#include <octoon/io/iostream.h>
#include <octoon/io/ioserver.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT vfstream final : public stream
		{
		public:
			vfstream() noexcept;
			vfstream(const vfstream&) noexcept = delete;
			vfstream(vfstream&&) noexcept;
			vfstream(std::shared_ptr<IoServer> fs) noexcept;

			vfstream& operator=(vfstream&&) noexcept;

			/*
			* Try to open the specified resource. If success, `can_read()`, `can_write()`
			* and `can_seek()` SHOULD report according to the capabilities inquired in
			* `options`. Actual reports depends on implementation of virtual directories
			* resources are in. If failed, these reports are always `false`.
			*
			* Any attempt to open a already-opened stream are allowed. After
			* re-opening, all information about the previous opening state is lost.
			*
			* Returns:
			*   `true` on success.
			*/
			bool open(const Orl& orl, const ios_base::open_mode mode) noexcept;
			void close() noexcept;

		private:
			std::shared_ptr<IoServer> fs_;
			std::unique_ptr<istream> stream_;
		};
	}
}

#endif
