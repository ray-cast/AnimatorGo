#ifndef OCTOON_OGG_AUDIO_READER_H_
#define OCTOON_OGG_AUDIO_READER_H_

#include <octoon/audio/audio_buffer.h>
#include <octoon/audio/audio_reader.h>
#include <filesystem>

struct OggVorbis_File;

namespace octoon
{
	class OCTOON_EXPORT OggStreamBuffer final : public AudioBuffer
	{
	public:
		OggStreamBuffer() noexcept;
		~OggStreamBuffer() noexcept;

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
		std::streamoff pos_;
		std::shared_ptr<io::istream> stream_;

		OggVorbis_File* oggVorbisFile_;
	};

	class OCTOON_EXPORT OggAudioReader final : public AudioReader
	{
		OctoonDeclareSubClass(OggAudioReader, AudioReader)
	public:
		OggAudioReader() noexcept;
		OggAudioReader(const std::filesystem::path& filepath) noexcept;
		virtual ~OggAudioReader() noexcept;

		bool open(const std::filesystem::path& filepath) noexcept(false);

		std::shared_ptr<AudioReader> clone() const noexcept override;

	private:
		OggStreamBuffer buf_;
	};
}

#endif