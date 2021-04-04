#ifndef OCTOON_AUDIO_LOADER_H_
#define OCTOON_AUDIO_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/audio/audio_reader.h>

namespace octoon
{
	class OCTOON_EXPORT AudioLoader final
	{
	public:
		AudioLoader() noexcept;
		~AudioLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		static std::shared_ptr<AudioReader> load(std::string_view filepath) noexcept(false);
		static std::shared_ptr<AudioReader> load(std::shared_ptr<io::istream> stream) noexcept(false);

		static void save(io::ostream& stream, const std::shared_ptr<AudioReader>& animation) noexcept(false);

	private:
		AudioLoader(const AudioLoader&) = delete;
		AudioLoader& operator=(const AudioLoader&) = delete;
	};
}

#endif