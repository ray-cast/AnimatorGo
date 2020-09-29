#include <octoon/audio/common/audio_reader.h>

namespace octoon
{
    AudioReader::AudioReader(AudioBuffer* buf) noexcept
		: io::istream(this->_buf)
    {
    }

    AudioReader::~AudioReader() noexcept
    {
    }

	bool
	AudioReader::open(std::shared_ptr<std::istream> stream) noexcept
	{
		return this->_buf->open(stream);
	}

	bool
	AudioReader::access(std::istream& stream) const noexcept
	{
		return this->_buf->access(stream);
	}

	std::uint8_t
	AudioReader::channel_count() const noexcept
	{
		return this->_buf->channel_count();
	}

	std::size_t
	AudioReader::total_samples() const noexcept
	{
		return this->_buf->total_samples();
	}

	AudioFormat
	AudioReader::type() const noexcept
	{
		return this->_buf->type();
	}

	AudioFrequency
	AudioReader::frequency() const noexcept
	{
		return this->_buf->frequency();
	}
}