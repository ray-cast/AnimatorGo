#ifndef OCTOON_RES_MANAGER_H_
#define OCTOON_RES_MANAGER_H_

#include <octoon/game_feature.h>
#include <octoon/graphics/graphics_types.h>
#include <octoon/runtime/singleton.h>
#include <octoon/model/mesh.h>
#include <map>

namespace octoon
{
	class OCTOON_EXPORT ResManager final
	{
		OctoonDeclareSingleton(ResManager)
	public:
		ResManager() noexcept;
		~ResManager() noexcept;

		GameObjectPtr createModel(const std::string& path, bool cache = true) except;
		graphics::GraphicsTexturePtr createTexture(const std::string& path, bool cache = true) except;

	private:
		using Prefabs = std::map<std::string, GameObjectPtr>;
		using Textures = std::map<std::string, graphics::GraphicsTexturePtr>;

		Prefabs _prefabs;
		Textures _textureCaches;
	};
}

#endif