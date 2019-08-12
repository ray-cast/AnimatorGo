#ifndef OCTOON_GAME_PREFABS_H_
#define OCTOON_GAME_PREFABS_H_

#include <octoon/game_object.h>
#include <octoon/runtime/singleton.h>
#include <octoon/hal/graphics_types.h>
#include <octoon/model/model.h>
#include <octoon/video/material.h>

#include <map>

namespace octoon
{
	class OCTOON_EXPORT GamePrefabs final
	{
		OctoonDeclareSingleton(GamePrefabs)
	public:
		GamePrefabs() noexcept;
		~GamePrefabs() noexcept;

		GameObjectPtr createOrthoCamera() except;
		GameObjectPtr createPerspectiveCamera(float fov) except;
		GameObjectPtr createFilmCamera(float canvasWidth, float zoom) except;

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
		GameObjectPtr createText(const char* u8str, std::uint16_t fontsize = 24, const char* fontPath = "../../system/fonts/DroidSansFallback.ttf") noexcept;
		GameObjectPtr createModel(const std::string& path, bool cache = true) except;
		GameObjectPtr createOfflineModel(const std::string& path, bool cache = true) except;
		GameObjectPtr createSprite(const std::string& image, float w, float h) except;
		GameObjectPtr createSprite(const hal::GraphicsTexturePtr& texture) except;
		GameObjectPtr createSprite(const hal::GraphicsTexturePtr& texture, float w, float h) except;
		GameObjectPtr createSpriteSquare(float w, float h) except;

		hal::GraphicsTexturePtr createTexture(const std::string& path, bool cache = true) noexcept;

	private:
		bool createBones(const model::Model& model, GameObjects& bones) noexcept;
		bool createSoftbodies(const model::Model& model, GameObjects& mesh, const GameObjects& bones, GameObjects& rigidbodys) noexcept;
		bool createRigidbodies(const model::Model& model, GameObjects& bones, GameObjects& rigidbodys) noexcept;
		bool createJoints(const model::Model& model, const GameObjects& rigidbodys, GameObjects& joints) noexcept;
		bool createSolver(const model::Model& model, GameObjects& bones) noexcept;
		bool createMeshes(const model::Model& model, GameObjects& mesh, const GameObjects& bones, const std::string& path = "") noexcept;
		bool createOfflineMeshes(const model::Model& model, GameObjects& mesh, const GameObjects& bones, const std::string& path = "") noexcept;
		bool createMaterials(const model::Model& model, video::Materials& materials, const std::string& rootPath = "") noexcept;

	private:
		using Prefabs = std::map<std::string, GameObjectPtr>;
		using Textures = std::map<std::string, hal::GraphicsTexturePtr>;

		Textures textureCaches_;
	};
}

#endif