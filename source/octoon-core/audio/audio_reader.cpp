#include <octoon/audio/audio_reader.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioReader, Object, "AudioReader")

	AudioReader::AudioReader(AudioBuffer* buf) noexcept
	{
		this->buf_ = buf;
	}

    AudioReader::~AudioReader() noexcept
    {
    }

	bool
	AudioReader::is_open() const noexcept
	{
		return this->buf_->is_open();
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

	std::streamsize
	AudioReader::read(char* str, std::streamsize cnt) noexcept
	{
		return this->buf_->read(str, cnt);
	}

	std::streamsize
	AudioReader::write(const char* str, std::streamsize cnt) noexcept
	{
		return this->buf_->write(str, cnt);
	}

	std::streamoff
	AudioReader::seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept
	{
		return this->buf_->seekg(pos, dir);
	}

	std::streamoff
	AudioReader::tellg() noexcept
	{
		return this->buf_->tellg();
	}

	std::streamsize
	AudioReader::size() const noexcept
	{
		return this->buf_->size();
	}

	int
	AudioReader::flush() noexcept
	{
		return this->buf_->flush();
	}

	void
	AudioReader::lock() noexcept
	{
		return this->buf_->lock();
	}

	void
	AudioReader::unlock() noexcept
	{
		return this->buf_->unlock();
	}
}