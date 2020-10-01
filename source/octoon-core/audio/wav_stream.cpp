#include <octoon/audio/wav_stream.h>
#include <octoon/io/vstream.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

namespace octoon
{
	WavStreamBuffer::WavStreamBuffer() noexcept
		: pos_(0)
	{
	}

	WavStreamBuffer::~WavStreamBuffer() noexcept
	{
		this->close();
	}

	bool
	WavStreamBuffer::access(io::istream& stream) const noexcept
	{
		return false;
	}

	void
	WavStreamBuffer::open(const char* filepath) noexcept(false)
	{
		io::ifstream stream(filepath);
		if (stream)
		{
			auto size = stream.size();
			auto buffer = std::make_unique<char[]>(size);
			if (stream.read(buffer.get(), size))
			{
				drwav* wav = new drwav;
				if (drwav_init_memory(wav, buffer.get(), size, nullptr))
				{
					this->hz_ = wav->sampleRate;
					this->channels_ = wav->channels;
					this->samples_= wav->totalPCMFrameCount;
					this->buffer_.resize(this->samples_ * this->channels_ * sizeof(drwav_int16));

					drwav_read_pcm_frames_s16(wav, this->samples_, (drwav_int16*)buffer_.data());
					drwav_uninit(wav);
				}
				else
				{
					throw std::runtime_error("Failed to read wav memory.");
				}
			}
		}
	}

	io::streamsize
	WavStreamBuffer::read(char* str, std::streamsize cnt) noexcept
	{
		auto buf_size = buffer_.size();
		std::size_t actual_copyable;

		if (static_cast<std::streamsize>(buf_size - pos_) > cnt)
		{
			actual_copyable = cnt;
			std::memcpy(str, buffer_.data() + pos_, actual_copyable);
			pos_ += cnt;
		}
		else
		{
			actual_copyable = buf_size - pos_;
			std::memcpy(str, buffer_.data() + pos_, actual_copyable);
			pos_ = buf_size;
		}

		return actual_copyable;
	}

	io::streamsize
	WavStreamBuffer::write(const char* str, std::streamsize cnt) noexcept
	{
		buffer_.reserve(pos_ + cnt);
		std::memcpy(buffer_.data() + pos_, str, cnt);
		return cnt;
	}

	io::streamoff
	WavStreamBuffer::seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept
	{
		assert(dir == io::ios_base::beg || dir == io::ios_base::cur || dir == io::ios_base::end);

		std::streamoff base = 0;
		switch (dir)
		{
		case std::ios_base::beg:
			base = 0;
			break;
		case std::ios_base::cur:
			base = pos_;
			break;
		case std::ios_base::end:
			base = this->size();
			break;
		}

		std::streamsize resultant = base + pos;
		if (resultant >= 0u && resultant <= this->size())
		{
			pos_ = resultant;
			return true;
		}

		return false;
	}

	io::streamoff
	WavStreamBuffer::tellg() noexcept
	{
		assert(this->is_open());
		return this->pos_;
	}

	io::streamsize
	WavStreamBuffer::size() const noexcept
	{
		return this->buffer_.size();
	}

	bool
	WavStreamBuffer::is_open() const noexcept
	{
		return !this->buffer_.empty();
	}

	int
	WavStreamBuffer::flush() noexcept
	{
		return 0;
	}

	bool
	WavStreamBuffer::close() noexcept
	{
		this->buffer_.shrink_to_fit();
		return true;
	}

	std::uint32_t
	WavStreamBuffer::channels() const noexcept
	{
		return this->channels_;
	}

	std::uint64_t
	WavStreamBuffer::samples() const noexcept
	{
		return this->samples_;
	}

	std::uint32_t
	WavStreamBuffer::frequency() const noexcept
	{
		return this->hz_;
	}

	WavAudioReader::WavAudioReader() noexcept
		: AudioReader(&buf_)
	{
	}

	WavAudioReader::WavAudioReader(const char* path) noexcept
		: AudioReader(&buf_)
	{
		this->open(path);
	}
	
	WavAudioReader::~WavAudioReader() noexcept
	{
	}

	void
	WavAudioReader::open(const char* path) noexcept(false)
	{
		return this->buf_.open(path);
	}

	std::shared_ptr<AudioReader>
	WavAudioReader::clone() const noexcept
	{
		return std::make_shared<WavAudioReader>();
	}
}