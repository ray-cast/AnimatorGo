#ifndef OCTOON_IO_MEMBUF_H_
#define OCTOON_IO_MEMBUF_H_

#include <mutex>
#include <vector>
#include <octoon/io/stream_buf.h>

namespace octoon
{
	namespace io
	{
		class membuf final : public stream_buf
		{
		public:
			membuf() noexcept;
			membuf(std::size_t capacity) noexcept;
			membuf(std::vector<std::uint8_t>&& buffer) noexcept;
			membuf(const std::vector<std::uint8_t>& buffer) noexcept;
			~membuf() noexcept;

			bool is_open() const noexcept;

			void open(std::size_t capacity) noexcept;
			void open(std::vector<std::uint8_t>&& buffer) noexcept;
			void open(const std::vector<std::uint8_t>& buffer) noexcept;

			bool close() noexcept;

			streamsize read(char* str, std::streamsize cnt) noexcept;
			streamsize write(const char* str, std::streamsize cnt) noexcept;

			streamoff seekg(ios_base::off_type pos, ios_base::seekdir dir) noexcept;
			streamoff tellg() noexcept;

			streamsize size() const noexcept;

			int flush() noexcept;

		private:
			std::size_t pos_;
			std::mutex lock_;
			std::vector<std::uint8_t> buffer_;
		};
	}
}

#endif