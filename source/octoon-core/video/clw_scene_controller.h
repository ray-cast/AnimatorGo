#ifndef OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_

#include <CLW.h>
#include <map>

#include <octoon/video/collector.h>
#include <octoon/material/mesh_standard_material.h>

#include "clw_scene.h"
#include "cl_program_manager.h"
#include "scene_controller.h"

namespace octoon::video
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

		void WriteLight(const std::shared_ptr<RenderScene>& scene, light::Light const& light, void* data) const;
		void WriteTexture(const hal::GraphicsTexture& texture, std::size_t data_offset, void* data) const;
		void WriteTextureData(hal::GraphicsTexture& texture, void* data) const;

		ClwScene::Material getMaterialIndex(const material::MaterialPtr& material) const;

	private:
		CLWContext context_;
		const CLProgramManager* programManager_;
		std::shared_ptr<RadeonRays::IntersectionApi> api_;
		std::unordered_map<void*, std::int32_t> textureToOffset_;
		std::unordered_map<void*, ClwScene::Material> materialidToOffset_;
		std::unordered_map<std::shared_ptr<RenderScene>, std::unique_ptr<ClwScene>> sceneCache_;

		Collector textureCollector;
		Collector materialCollector;
	};
}

#endif