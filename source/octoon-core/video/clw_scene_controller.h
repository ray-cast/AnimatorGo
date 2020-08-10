#ifndef OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_

#include <CLW.h>
#include <map>

#include "clw_scene.h"
#include "scene_controller.h"

namespace octoon::video
{
	class ClwSceneController : public SceneController
	{
	public:
		ClwSceneController(const CLWContext& context, const std::shared_ptr<RadeonRays::IntersectionApi>& api);

		void compileScene(RenderScene* scene) noexcept override;
		CompiledScene& getCachedScene(const RenderScene* scene) const noexcept(false);

	private:
		void updateCamera(const RenderScene* scene, ClwScene& out) const;
		void updateShapes(const RenderScene* scene, ClwScene& out) const;
		void updateIntersector(const RenderScene* scene, ClwScene& out) const;

	private:
		CLWContext context_;
		std::shared_ptr<RadeonRays::IntersectionApi> api_;
		std::unordered_map<const RenderScene*, std::unique_ptr<ClwScene>> sceneCache_;
	};
}

#endif