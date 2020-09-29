#ifndef OCTOON_OGG_AUDIO_BUFFER_H_
#define OCTOON_OGG_AUDIO_BUFFER_H_

#include <octoon/audio/common/audio_buffer.h>

struct OggVorbis_File;

namespace octoon
{
	class OggStreamBuffer final : public AudioBuffer
	{
	public:
		OggStreamBuffer() noexcept;
		~OggStreamBuffer() noexcept;

		virtual bool open(std::shared_ptr<io::istream> stream) noexcept;
		virtual bool access(io::istream& stream) const noexcept;

		virtual io::streamsize read(char* str, io::streamsize cnt) noexcept;
		virtual io::streamsize write(const char* str, io::streamsize cnt) noexcept;

		virtual io::streamoff seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept;
		virtual io::streamoff tellg() noexcept;

		virtual io::streamsize size() const noexcept;

		virtual bool is_open() const noexcept;

		virtual int flush() noexcept;

		virtual bool close() noexcept;

		virtual std::uint32_t getBufferChannelCount() const noexcept;
		virtual std::size_t getBufferTotalSamples() const noexcept;

		virtual AudioFormat getBufferType() const noexcept;
		virtual AudioFrequency getBufferFrequency() const noexcept;

	private:

		std::streamoff _next;
		std::shared_ptr<io::istream> _stream;

		OggVorbis_File* _oggVorbisFile;
	};
}

#endif