#ifndef OCTOON_ASS_LOADER_H_
#define OCTOON_ASS_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT ASSLoader final
	{
	public:
		ASSLoader() noexcept;
		~ASSLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		static GameObjects load(std::string_view filepath) noexcept(false);

	private:
		ASSLoader(const ASSLoader&) = delete;
		ASSLoader& operator=(const ASSLoader&) = delete;
	};
}

#endif