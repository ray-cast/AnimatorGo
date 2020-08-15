#ifndef OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_

#include <CLW.h>
#include <map>

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

		void compileScene(RenderScene* scene) noexcept override;
		CompiledScene& getCachedScene(const RenderScene* scene) const noexcept(false);

	private:
		void updateCamera(const RenderScene* scene, ClwScene& out) const;
		void updateTextures(const RenderScene* scene, ClwScene& out);
		void updateMaterials(const RenderScene* scene, ClwScene& out);
		void updateShapes(const RenderScene* scene, ClwScene& out) const;
		void updateIntersector(const RenderScene* scene, ClwScene& out) const;

		ClwScene::Material getMaterialIndex(const material::MaterialPtr& material) const;

	private:
		CLWContext context_;
		const CLProgramManager* programManager_;
		std::shared_ptr<RadeonRays::IntersectionApi> api_;
		std::unordered_map<void*, std::int32_t> textureToOffset_;
		std::unordered_map<void*, ClwScene::Material> materialidToOffset_;
		std::unordered_map<const RenderScene*, std::unique_ptr<ClwScene>> sceneCache_;
	};
}

#endif