// File: mstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_MSTREAM_H_
#define OCTOON_IO_MSTREAM_H_

#include <mutex>
#include <octoon/io/stream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT mstream final : public stream
		{
		public:
			mstream() noexcept;
			mstream(const mstream&) = delete;
			mstream(mstream&& rv) noexcept;
			mstream(std::size_t capacity) noexcept;
			mstream(std::vector<std::uint8_t> buffer) noexcept;

			mstream& operator=(mstream&& rv) noexcept;

			bool can_read() const noexcept override;
			bool can_write() const noexcept override;
			bool can_seek() const noexcept override;

			std::size_t read(std::uint8_t* buf, std::size_t size) override;
			std::size_t write(const uint8_t* buf, std::size_t size) override;
			bool seek(long dist, ios_base::seek_dir seek = ios_base::cur) override;

			std::vector<std::uint8_t> into_buffer() noexcept;

		private:
			std::size_t pos_;
			std::mutex lock_;
			std::vector<std::uint8_t> buffer_;
		};
	}
}

#endif
