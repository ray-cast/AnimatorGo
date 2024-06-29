#ifndef OCTOON_SCENE_CONTROLLER_H_
#define OCTOON_SCENE_CONTROLLER_H_

#include <octoon/video/render_scene.h>
#include <octoon/video/compiled_scene.h>

namespace octoon
{
	class SceneController
	{
	public:
		SceneController() = default;
		virtual ~SceneController() = default;

		virtual void cleanCache() noexcept = 0;
		virtual void compileScene(const std::shared_ptr<RenderScene>& scene) noexcept = 0;
		virtual CompiledScene& getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false) = 0;
	};
}

#endif