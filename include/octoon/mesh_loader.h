#ifndef OCTOON_MESH_LOADER_H_
#define OCTOON_MESH_LOADER_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT MeshLoader final
	{
	public:
		static GameObjectPtr load(const std::string_view& path, bool cache = true) noexcept(false);
	};
}

#endif