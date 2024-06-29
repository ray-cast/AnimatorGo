// File: vstream.h
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
		class OCTOON_EXPORT ivstream final : public istream
		{
		public:
			ivstream() noexcept;
			ivstream(const Orl& orl, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			ivstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			ivstream& open(const Orl& orl, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream& open(const char* path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::in) noexcept;
			ivstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ivstream(const ivstream&) noexcept = delete;
			ivstream operator=(const ivstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};

		class OCTOON_EXPORT ovstream final : public ostream
		{
		public:
			ovstream() noexcept;
			ovstream(const Orl& orl, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			ovstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			ovstream& open(const Orl& orl, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream& open(const char* path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::out) noexcept;
			ovstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			ovstream(const ovstream&) noexcept = delete;
			ovstream operator=(const ovstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};

		class OCTOON_EXPORT vstream final : public iostream
		{
		public:
			vstream() noexcept;
			vstream(const Orl& orl, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream(std::shared_ptr<IoServer>&& filesystem) noexcept;
			vstream(const std::shared_ptr<IoServer>& filesystem) noexcept;

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
			vstream& open(const Orl& orl, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream& open(const char* path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream& open(const std::string& path, const ios_base::open_mode mode = ios_base::in | ios_base::out) noexcept;
			vstream& close() noexcept;

			bool is_open() const noexcept;

		private:
			vstream(const vstream&) noexcept = delete;
			vstream operator=(const vstream&) noexcept = delete;

		private:
			virtual_buf buf_;
			std::shared_ptr<IoServer> filesystem_;
		};
	}
}

#endif
