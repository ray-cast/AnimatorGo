#include <octoon/audio_loader.h>
#include <octoon/audio/ogg_stream.h>
#include <octoon/audio/mp3_stream.h>
#include <octoon/audio/wav_stream.h>
#include <octoon/audio/flac_stream.h>
#include <octoon/io/vstream.h>

namespace octoon
{
	AudioLoader::AudioLoader() noexcept
	{
	}

	AudioLoader::~AudioLoader() noexcept
	{
	}

	bool
	AudioLoader::doCanRead(io::istream& stream) noexcept
	{
		return false;
	}

	bool
	AudioLoader::doCanRead(const char* type) noexcept
	{
		return false;
	}

	std::shared_ptr<AudioReader>
	AudioLoader::load(std::shared_ptr<io::istream> stream) noexcept(false)
	{
		return nullptr;
	}

	std::shared_ptr<AudioReader>
	AudioLoader::load(std::string_view filepath) noexcept(false)
	{
		if (filepath.find(".ogg") != std::string::npos)
			return load(std::make_shared<io::ivstream>(std::string(filepath)));
		else if (filepath.find(".mp3") != std::string::npos)
			return std::make_shared<Mp3AudioReader>(std::string(filepath).c_str());
		else if (filepath.find(".wav") != std::string::npos)
			return std::make_shared<WavAudioReader>(std::string(filepath).c_str());
		else if (filepath.find(".flac") != std::string::npos)
			return std::make_shared<FlacAudioReader>(std::string(filepath).c_str());

		return nullptr;
	}

	void
	AudioLoader::save(io::ostream& stream, const std::shared_ptr<AudioReader>& animation) noexcept(false)
	{
		
	}
}