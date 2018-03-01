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
		class OCTOON_EXPORT mstream : public stream
		{
		public:
			mstream() noexcept;
			mstream(const mstream&) = delete;
			mstream(mstream&& rv) noexcept;
			mstream(std::size_t capacity) noexcept;
			mstream(std::vector<uint8_t> buffer) noexcept;

			mstream& operator=(mstream&& rv) noexcept;

			bool can_read() const noexcept override final;
			bool can_write() const noexcept override final;
			bool can_seek() const noexcept override final;

			size_t read(uint8_t* buf, size_t size) override final;
			size_t write(const uint8_t* buf, size_t size) override final;
			bool seek(long dist, ios_base::seek_dir seek = ios_base::cur) override final;

			std::vector<uint8_t> into_buffer() noexcept;

		private:
			std::vector<uint8_t> buffer_;
			std::mutex lock_;
			size_t pos_;
		};
	}
}

#endif
