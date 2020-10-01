#ifndef OCTOON_FLAC_AUDIO_BUFFER_H_
#define OCTOON_FLAC_AUDIO_BUFFER_H_

#include <octoon/audio/audio_reader.h>

namespace octoon
{
	class FlacStreamBuffer final : public AudioBuffer
	{
	public:
		FlacStreamBuffer() noexcept;
		~FlacStreamBuffer() noexcept;

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

	class OCTOON_EXPORT FlacAudioReader final : public AudioReader
	{
	public:
		FlacAudioReader() noexcept;
		FlacAudioReader(const char* path) noexcept;
		virtual ~FlacAudioReader() noexcept;

		void open(const char* path) noexcept(false);

		std::shared_ptr<AudioReader> clone() const noexcept override;

	private:
		FlacStreamBuffer buf_;
	};
}

#endif