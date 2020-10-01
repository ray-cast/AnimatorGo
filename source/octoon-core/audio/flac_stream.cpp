#include <octoon/audio/flac_stream.h>
#include <octoon/io/vstream.h>
#define DR_FLAC_IMPLEMENTATION
#include <dr_flac.h>

namespace octoon
{
	FlacStreamBuffer::FlacStreamBuffer() noexcept
		: pos_(0)
	{
	}

	FlacStreamBuffer::~FlacStreamBuffer() noexcept
	{
		this->close();
	}

	bool
	FlacStreamBuffer::access(io::istream& stream) const noexcept
	{
		return false;
	}

	void
	FlacStreamBuffer::open(const char* filepath) noexcept(false)
	{
		io::ifstream stream(filepath);
		if (stream)
		{
			auto size = stream.size();
			auto buffer = std::make_unique<char[]>(size);
			if (stream.read(buffer.get(), size))
			{
				drflac* flac = drflac_open_memory(buffer.get(), size, nullptr);
				if (flac)
				{
					this->hz_ = flac->sampleRate;
					this->channels_ = flac->channels;
					this->samples_= flac->totalPCMFrameCount;
					this->buffer_.resize(this->samples_ * this->channels_ * sizeof(drflac_int32));

					drflac_read_pcm_frames_s32(flac, this->samples_, (drflac_int32*)buffer_.data());
					drflac_close(flac);
				}
				else
				{
					throw std::runtime_error("Failed to read flac memory.");
				}
			}
		}
	}

	io::streamsize
	FlacStreamBuffer::read(char* str, std::streamsize cnt) noexcept
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
	FlacStreamBuffer::write(const char* str, std::streamsize cnt) noexcept
	{
		buffer_.reserve(pos_ + cnt);
		std::memcpy(buffer_.data() + pos_, str, cnt);
		return cnt;
	}

	io::streamoff
	FlacStreamBuffer::seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept
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
	FlacStreamBuffer::tellg() noexcept
	{
		assert(this->is_open());
		return this->pos_;
	}

	io::streamsize
	FlacStreamBuffer::size() const noexcept
	{
		return this->buffer_.size();
	}

	bool
	FlacStreamBuffer::is_open() const noexcept
	{
		return !this->buffer_.empty();
	}

	int
	FlacStreamBuffer::flush() noexcept
	{
		return 0;
	}

	bool
	FlacStreamBuffer::close() noexcept
	{
		this->buffer_.shrink_to_fit();
		return true;
	}

	std::uint32_t
	FlacStreamBuffer::channels() const noexcept
	{
		return this->channels_;
	}

	std::uint64_t
	FlacStreamBuffer::samples() const noexcept
	{
		return this->samples_;
	}

	std::uint32_t
	FlacStreamBuffer::frequency() const noexcept
	{
		return this->hz_;
	}

	FlacAudioReader::FlacAudioReader() noexcept
		: AudioReader(&buf_)
	{
	}

	FlacAudioReader::FlacAudioReader(const char* path) noexcept
		: AudioReader(&buf_)
	{
		this->open(path);
	}
	
	FlacAudioReader::~FlacAudioReader() noexcept
	{
	}

	void
	FlacAudioReader::open(const char* path) noexcept(false)
	{
		return this->buf_.open(path);
	}

	std::shared_ptr<AudioReader>
	FlacAudioReader::clone() const noexcept
	{
		return std::make_shared<FlacAudioReader>();
	}
}