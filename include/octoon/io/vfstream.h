// File: vfstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_VFSTREAM_H_
#define OCTOON_IO_VFSTREAM_H_

#include <octoon/io/iostream.h>
#include <octoon/io/ioserver.h>
#include <octoon/io/virtual_buf.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT ivfstream final : public istream
		{
		public:
			ivfstream() noexcept;
			ivfstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			ivfstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			ivfstream& open(const Orl& orl, const ios_base::open_mode mode) noexcept;
			ivfstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ivfstream(const ivfstream&) noexcept = delete;
			ivfstream operator=(const ivfstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};

		class OCTOON_EXPORT ovfstream final : public ostream
		{
		public:
			ovfstream() noexcept;
			ovfstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			ovfstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			ovfstream& open(const Orl& orl, const ios_base::open_mode mode) noexcept;
			ovfstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ovfstream(const ovfstream&) noexcept = delete;
			ovfstream operator=(const ovfstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};

		class OCTOON_EXPORT vfstream final : public iostream
		{
		public:
			vfstream() noexcept;
			vfstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			vfstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			vfstream& open(const Orl& orl, const ios_base::open_mode mode) noexcept;
			vfstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			vfstream(const vfstream&) noexcept = delete;
			vfstream operator=(const vfstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};
	}
}

#endif
