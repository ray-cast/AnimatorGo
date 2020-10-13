#ifndef OCTOON_OBJ_LOADER_H_
#define OCTOON_OBJ_LOADER_H_

#include <octoon/io/iostream.h>
#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT OBJLoader final
	{
	public:
		OBJLoader() noexcept;
		~OBJLoader() noexcept;

		static bool doCanRead(io::istream& stream) noexcept;
		static bool doCanRead(const char* type) noexcept;

		static GameObjects load(std::string_view filepath) noexcept(false);

	private:
		OBJLoader(const OBJLoader&) = delete;
		OBJLoader& operator=(const OBJLoader&) = delete;
	};
}

#endif