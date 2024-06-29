#ifndef OCTOON_CLW_SCENE_CONTROLLER_H_
#define OCTOON_CLW_SCENE_CONTROLLER_H_

#include <CLW.h>
#include <map>
#include <optional>

#include <octoon/video/collector.h>
#include <octoon/video/scene_controller.h>
#include <octoon/material/mesh_standard_material.h>

#include "clw_scene.h"
#include "cl_program_manager.h"

namespace octoon
{
	class ClwSceneController : public SceneController
	{
	public:
		ClwSceneController(const CLWContext& context, const std::shared_ptr<RadeonRays::IntersectionApi>& api, const CLProgramManager* program_manager);

		void cleanCache() noexcept;
		void compileScene(const std::shared_ptr<RenderScene>& scene) noexcept override;
		CompiledScene& getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false);

	private:
		void updateCamera(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const;
		void updateTextures(const std::shared_ptr<RenderScene>& scene, ClwScene& out);
		void updateMaterials(const std::shared_ptr<RenderScene>& scene, ClwScene& out);
		void updateShapes(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const;
		void updateIntersector(const std::shared_ptr<RenderScene>& scene, ClwScene& out) const;
		void updateLights(const std::shared_ptr<RenderScene>& scene, ClwScene& out);

		void WriteLight(const std::shared_ptr<RenderScene>& scene, Light const& light, void* data) const;
		void WriteTexture(const Texture& texture, std::size_t data_offset, void* data) const;
		void WriteTextureData(Texture& texture, void* data) const;

		std::optional<ClwScene::Material> getMaterialIndex(const MaterialPtr& material) const;

	private:
		CLWContext context_;
		const CLProgramManager* programManager_;
		std::shared_ptr<RadeonRays::IntersectionApi> api_;
		std::unordered_map<void*, ClwScene::Material> materialidToOffset_;
		std::unordered_map<std::shared_ptr<RenderScene>, std::unique_ptr<ClwScene>> sceneCache_;

		Collector textureCollector;
		Collector materialCollector;
	};
}

#endif