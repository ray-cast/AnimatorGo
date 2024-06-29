#ifndef OCTOON_WAV_AUDIO_READER_H_
#define OCTOON_WAV_AUDIO_READER_H_

#include <octoon/audio/audio_reader.h>
#include <filesystem>

namespace octoon
{
	class OCTOON_EXPORT WavStreamBuffer final : public AudioBuffer
	{
	public:
		WavStreamBuffer() noexcept;
		~WavStreamBuffer() noexcept;

		virtual bool open(const std::filesystem::path& filepath) noexcept(false);
		virtual bool access(io::istream& stream) const noexcept;

		virtual io::streamsize read(char* str, io::streamsize cnt) noexcept;
		virtual io::streamsize write(const char* str, io::streamsize cnt) noexcept;

		virtual io::streamoff seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept;
		virtual io::streamoff tellg() noexcept;

		virtual io::streamsize size() const noexcept;

		virtual bool is_open() const noexcept;

		virtual int flush() noexcept;

		virtual bool close() noexcept;

		virtual std::uint16_t bitsPerSample() const noexcept override;
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

	class OCTOON_EXPORT WavAudioReader final : public AudioReader
	{
		OctoonDeclareSubClass(WavAudioReader, AudioReader)
	public:
		WavAudioReader() noexcept;
		WavAudioReader(const std::filesystem::path& path) noexcept;
		virtual ~WavAudioReader() noexcept;

		bool open(const std::filesystem::path& path) noexcept(false);

		std::shared_ptr<AudioReader> clone() const noexcept override;

	private:
		WavStreamBuffer buf_;
	};
}

#endif