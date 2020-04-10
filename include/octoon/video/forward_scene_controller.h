#ifndef OCTOON_VIDEO_FORWARD_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_FORWARD_SCENE_CONTROLLER_H_

#include <octoon/hal/graphics_context.h>
#include <map>

#include "forward_scene.h"
#include "scene_controller.h"

namespace octoon::video
{
	class ForwardSceneController : public SceneController
	{
	public:
		ForwardSceneController(const hal::GraphicsContextPtr& context);

		void compileScene(RenderScene* scene) noexcept override;
		CompiledScene& getCachedScene(RenderScene* scene) const noexcept(false);

	private:
		void updateCamera(const RenderScene* scene, ForwardScene& out) const;
		void updateIntersector(const RenderScene* scene, ForwardScene& out) const;
		void updateLights(const RenderScene* scene, ForwardScene& out) noexcept;

	private:
		hal::GraphicsContextPtr context_;
		std::unordered_map<RenderScene*, std::unique_ptr<ForwardScene>> sceneCache_;
	};
}

#endif