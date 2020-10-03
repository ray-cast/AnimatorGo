#include <octoon/audio/audio_reader.h>

namespace octoon
{
	AudioReader::AudioReader(AudioBuffer* buf) noexcept
		: io::istream(buf)
	{
		this->buf_ = buf;
	}

    AudioReader::~AudioReader() noexcept
    {
    }

	bool
	AudioReader::access(io::istream& stream) const noexcept
	{
		return this->buf_->access(stream);
	}

	std::uint32_t
	AudioReader::channels() const noexcept
	{
		return this->buf_->channels();
	}

	std::uint64_t
	AudioReader::samples() const noexcept
	{
		return this->buf_->samples();
	}

	std::uint32_t
	AudioReader::frequency() const noexcept
	{
		return this->buf_->frequency();
	}

	std::uint16_t
	AudioReader::bitsPerSample() const noexcept
	{
		return this->buf_->bitsPerSample();
	}
}