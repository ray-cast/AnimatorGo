// File: fstream.h
// Author: PENGUINLIONG
#ifndef OCTOON_IO_FSTREAM_H_
#define OCTOON_IO_FSTREAM_H_

#include <memory>
#include <fstream>

#include <octoon/io/ori.h>
#include <octoon/io/stream.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT fstream final : public stream
		{
		public:
			bool open(const std::string& path, const ios_base::open_mode mode) noexcept;

			bool can_read() const noexcept override;
			bool can_write() const noexcept override;
			bool can_seek() const noexcept override;

			std::size_t read(uint8_t* buf, std::size_t size) noexcept override;
			std::size_t write(const uint8_t* buf, std::size_t size) noexcept override;

			bool seek(long dist, ios_base::seek_dir seek) noexcept override;

		private:
			std::fstream stream_;
			ios_base::open_mode opts_;
		};
	}
}

#endif
