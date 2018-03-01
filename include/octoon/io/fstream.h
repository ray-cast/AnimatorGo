// File: fstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_

#include <memory>

#include <octoon/io/ori.h>
#include <octoon/io/stream.h>

namespace octoon
{
	namespace io
	{
		class IoServer;

		class OCTOON_EXPORT fstream final : public stream
		{
		public:
			fstream() noexcept;
			fstream(const fstream&) noexcept = delete;
			fstream(fstream&&) noexcept;
			fstream(std::shared_ptr<IoServer> fs) noexcept;

			fstream& operator=(fstream&&) noexcept;

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

			bool can_read() const noexcept override;
			bool can_write() const noexcept override;
			bool can_seek() const noexcept override;

			std::size_t read(uint8_t* buf, std::size_t size) override final;
			std::size_t write(const uint8_t* buf, std::size_t size) override final;

			bool seek(long dist, ios_base::seek_dir seek = ios_base::cur) override final;

		private:
			std::shared_ptr<IoServer> fs_;
			std::unique_ptr<stream> stream_;
		};
	}
}

#endif
