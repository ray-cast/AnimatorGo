#ifndef OCTOON_SCRIPTABLE_SCENE_CONTROLLER_H_
#define OCTOON_SCRIPTABLE_SCENE_CONTROLLER_H_

#include <octoon/hal/graphics_context.h>
#include <octoon/mesh/mesh.h>
#include <octoon/video/collector.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/rendering_data.h>
#include <octoon/video/scene_controller.h>

#include <unordered_map>

namespace octoon
{
	class OCTOON_EXPORT ScriptableSceneController
	{
	public:
		ScriptableSceneController();
		ScriptableSceneController(const GraphicsContextPtr& context);
		~ScriptableSceneController();

		void cleanCache() noexcept;
		void compileScene(const std::shared_ptr<RenderScene>& scene) noexcept;
		RenderingData& getCachedScene(const std::shared_ptr<RenderScene>& scene) const noexcept(false);

	private:
		void updateCamera(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateLights(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateMaterials(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);
		void updateShapes(const std::shared_ptr<RenderScene>& scene, class RenderingData& out, bool force = false);

	private:
		GraphicsContextPtr context_;

		Collector materialCollector;
		std::unordered_map<std::shared_ptr<RenderScene>, std::shared_ptr<RenderingData>> sceneCache_;
	};
}

#endif
