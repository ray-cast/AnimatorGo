#include <octoon/audio/mp3_stream.h>
#include <octoon/io/vstream.h>
#define DR_MP3_IMPLEMENTATION
#include <dr_mp3.h>

namespace octoon
{
	Mp3StreamBuffer::Mp3StreamBuffer() noexcept
		: pos_(0)
	{
	}

	Mp3StreamBuffer::~Mp3StreamBuffer() noexcept
	{
		this->close();
	}

	bool
	Mp3StreamBuffer::access(io::istream& stream) const noexcept
	{
		return false;
	}

	void
	Mp3StreamBuffer::open(const char* filepath) noexcept(false)
	{
		io::ifstream stream(filepath);
		if (stream)
		{
			auto size = stream.size();
			auto buffer = std::make_unique<char[]>(size);
			if (stream.read(buffer.get(), size))
			{
				drmp3* mp3 = new drmp3;
				if (drmp3_init_memory(mp3, buffer.get(), size, nullptr))
				{
					this->hz_ = mp3->sampleRate;
					this->channels_ = mp3->channels;
					this->samples_= drmp3_get_pcm_frame_count(mp3);
					this->buffer_.resize(this->samples_ * this->channels_ * sizeof(drmp3_int16));

					drmp3_read_pcm_frames_s16(mp3, this->samples_, (drmp3_int16*)buffer_.data());
					drmp3_uninit(mp3);
				}
				else
				{
					throw std::runtime_error("Failed to read mp3 memory.");
				}
			}
		}
	}

	io::streamsize
	Mp3StreamBuffer::read(char* str, std::streamsize cnt) noexcept
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
	Mp3StreamBuffer::write(const char* str, std::streamsize cnt) noexcept
	{
		buffer_.reserve(pos_ + cnt);
		std::memcpy(buffer_.data() + pos_, str, cnt);
		return cnt;
	}

	io::streamoff
	Mp3StreamBuffer::seekg(io::ios_base::off_type pos, io::ios_base::seekdir dir) noexcept
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
	Mp3StreamBuffer::tellg() noexcept
	{
		assert(this->is_open());
		return this->pos_;
	}

	io::streamsize
	Mp3StreamBuffer::size() const noexcept
	{
		return this->buffer_.size();
	}

	bool
	Mp3StreamBuffer::is_open() const noexcept
	{
		return !this->buffer_.empty();
	}

	int
	Mp3StreamBuffer::flush() noexcept
	{
		return 0;
	}

	bool
	Mp3StreamBuffer::close() noexcept
	{
		this->buffer_.shrink_to_fit();
		return true;
	}

	std::uint32_t
	Mp3StreamBuffer::channels() const noexcept
	{
		return this->channels_;
	}

	std::uint64_t
	Mp3StreamBuffer::samples() const noexcept
	{
		return this->samples_;
	}

	std::uint32_t
	Mp3StreamBuffer::frequency() const noexcept
	{
		return this->hz_;
	}

	std::uint16_t
	Mp3StreamBuffer::bitsPerSample() const noexcept
	{
		return 16;
	}

	Mp3AudioReader::Mp3AudioReader() noexcept
		: AudioReader(&buf_)
	{
	}

	Mp3AudioReader::Mp3AudioReader(const char* path) noexcept
		: AudioReader(&buf_)
	{
		this->open(path);
	}
	
	Mp3AudioReader::~Mp3AudioReader() noexcept
	{
	}

	void
	Mp3AudioReader::open(const char* path) noexcept(false)
	{
		return this->buf_.open(path);
	}

	std::shared_ptr<AudioReader>
	Mp3AudioReader::clone() const noexcept
	{
		return std::make_shared<Mp3AudioReader>();
	}
}