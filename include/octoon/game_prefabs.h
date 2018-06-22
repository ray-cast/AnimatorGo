#ifndef OCTOON_GAME_PREFABS_H_
#define OCTOON_GAME_PREFABS_H_

#include <octoon/game_types.h>
#include <octoon/runtime/singleton.h>
#include <octoon/graphics/graphics_types.h>

#include <map>

namespace octoon
{
	class OCTOON_EXPORT GamePrefabs final
	{
		OctoonDeclareSingleton(GamePrefabs)
	public:
		GamePrefabs() noexcept;
		~GamePrefabs() noexcept;

		GameObjectPtr createCamera() except;
		GameObjectPtr createCamera2D() except;

		GameObjectPtr createCircle(float radius, std::uint32_t segments, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;
		GameObjectPtr createPlane(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
		GameObjectPtr createPlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept;
		GameObjectPtr createPlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear = true) noexcept;
		GameObjectPtr createFloor(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
		GameObjectPtr createNoise(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
		GameObjectPtr createCube(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
		GameObjectPtr createCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
		GameObjectPtr createRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart = 0, float thetaLength = math::PI) noexcept;
		GameObjectPtr createSphere(float radius, std::uint32_t widthSegments = 32, std::uint32_t heightSegments = 24, float phiStart = 0, float phiLength = math::PI_2, float thetaStart = 0, float thetaLength = math::PI) noexcept;
		GameObjectPtr createVolumes(float fovy, float znear, float zfar) noexcept;
		GameObjectPtr createCone(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;
		GameObjectPtr createText(const wchar_t* text, std::uint16_t fontsize = 24, const char* fontPath = "../../system/fonts/DroidSansFallback.ttf") noexcept;
		GameObjectPtr createText(const char* text, std::uint16_t fontsize = 24, const char* fontPath = "../../system/fonts/DroidSansFallback.ttf") noexcept;
		GameObjectPtr createModel(const std::string& path, bool cache = true) except;

		GameObjectPtr createSprite(const std::string& image) except;
		GameObjectPtr createSprite(const graphics::GraphicsTexturePtr& texture) except;
		GameObjectPtr createSpriteSquare() except;

		graphics::GraphicsTexturePtr createTexture(const std::string& path, bool cache = true) except;

	private:
		using Prefabs = std::map<std::string, GameObjectPtr>;
		using Textures = std::map<std::string, graphics::GraphicsTexturePtr>;

		Prefabs prefabs_;
		Textures textureCaches_;
	};
}

#endif