#include <octoon/audio/common/audio_reader.h>

namespace octoon
{
	AudioReader::AudioReader() noexcept
		: io::istream(&this->_buf)
	{
	}

    AudioReader::AudioReader(std::shared_ptr<io::istream> stream) noexcept
		: io::istream(&this->_buf)
    {
		this->open(stream);
    }

    AudioReader::~AudioReader() noexcept
    {
    }

	bool
	AudioReader::open(std::shared_ptr<io::istream> stream) noexcept
	{
		return this->_buf.open(stream);
	}

	bool
	AudioReader::access(io::istream& stream) const noexcept
	{
		return this->_buf.access(stream);
	}

	std::uint8_t
	AudioReader::channel_count() const noexcept
	{
		return this->_buf.channel_count();
	}

	std::size_t
	AudioReader::total_samples() const noexcept
	{
		return this->_buf.total_samples();
	}

	AudioFormat
	AudioReader::type() const noexcept
	{
		return this->_buf.type();
	}

	AudioFrequency
	AudioReader::frequency() const noexcept
	{
		return this->_buf.frequency();
	}

	std::shared_ptr<AudioReader>
	AudioReader::clone() const noexcept
	{
		return std::make_shared<AudioReader>();
	}
}