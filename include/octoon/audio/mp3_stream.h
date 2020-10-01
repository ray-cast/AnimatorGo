#ifndef OCTOON_MP3_AUDIO_BUFFER_H_
#define OCTOON_MP3_AUDIO_BUFFER_H_

#include <octoon/audio/audio_reader.h>

namespace octoon
{
	class Mp3StreamBuffer final : public AudioBuffer
	{
	public:
		Mp3StreamBuffer() noexcept;
		~Mp3StreamBuffer() noexcept;

		virtual void open(const char* filepath) noexcept(false);
		virtual bool access(io::istream& stream) const noexcept;

		virtual io::streamsize read(char* str, io::streamsize cnt) noexcept;
		virtual io::streamsize write(const char* str, io::streamsize cnt) noexcept;

		virtual io::streamoff seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept;
		virtual io::streamoff tellg() noexcept;

		virtual io::streamsize size() const noexcept;

		virtual bool is_open() const noexcept;

		virtual int flush() noexcept;

		virtual bool close() noexcept;

		virtual std::uint32_t channels() const noexcept override;
		virtual std::uint64_t samples() const noexcept override;
		virtual std::uint32_t frequency() const noexcept override;

	private:
		std::uint32_t hz_;
		std::uint32_t channels_;
		std::uint64_t samples_;
		std::streamoff pos_;
		std::vector<std::uint8_t> buffer_;
	};

	class OCTOON_EXPORT Mp3AudioReader final : public AudioReader
	{
	public:
		Mp3AudioReader() noexcept;
		Mp3AudioReader(const char* path) noexcept;
		virtual ~Mp3AudioReader() noexcept;

		void open(const char* path) noexcept(false);

		std::shared_ptr<AudioReader> clone() const noexcept override;

	private:
		Mp3StreamBuffer buf_;
	};
}

#endif