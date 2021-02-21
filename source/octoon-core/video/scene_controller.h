#ifndef OCTOON_VIDEO_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_SCENE_CONTROLLER_H_

#include <octoon/video/render_scene.h>
#include <octoon/video/compiled_scene.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon::video
{
	class SceneController
	{
	public:
		SceneController();
		virtual ~SceneController();

		virtual void cleanCache() noexcept = 0;
		virtual void compileScene(const std::shared_ptr<ScriptableRenderContext>& context, const std::shared_ptr<RenderScene>& scene) noexcept = 0;
		virtual CompiledScene& getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false) = 0;
	};
}

#endif