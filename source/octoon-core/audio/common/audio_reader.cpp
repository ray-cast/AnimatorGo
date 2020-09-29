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
	AudioReader::get_buffer_channel_count() const noexcept
	{
		return this->_buf->get_buffer_channel_count();
	}

	std::size_t
	AudioReader::get_buffer_total_samples() const noexcept
	{
		return this->_buf->get_buffer_total_samples();
	}

	AudioFormat
	AudioReader::get_buffer_type() const noexcept
	{
		return this->_buf->get_buffer_type();
	}

	AudioFrequency
	AudioReader::get_buffer_frequency() const noexcept
	{
		return this->_buf->get_buffer_frequency();
	}
}