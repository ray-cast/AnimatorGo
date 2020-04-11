#ifndef OCTOON_VIDEO_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_SCENE_CONTROLLER_H_

#include <octoon/video/render_scene.h>
#include <octoon/video/compiled_scene.h>

namespace octoon::video
{
	class SceneController
	{
	public:
		SceneController();

		virtual void compileScene(RenderScene* scene) noexcept = 0;
		virtual CompiledScene& getCachedScene(const RenderScene* scene) const noexcept(false) = 0;
	};
}

#endif