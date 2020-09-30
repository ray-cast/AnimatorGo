#include <octoon/audio_loader.h>
#include <octoon/audio/common/ogg_stream.h>
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
		return std::make_shared<AudioReader>(stream);
	}

	std::shared_ptr<AudioReader>
	AudioLoader::load(std::string_view filepath) noexcept(false)
	{
		auto stream = std::make_shared<io::ivstream>(std::string(filepath));
		return load(stream);
	}

	void
	AudioLoader::save(io::ostream& stream, const std::shared_ptr<AudioReader>& animation) noexcept(false)
	{
		
	}
}