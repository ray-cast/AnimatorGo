#ifndef OCTOON_RENDER_SCENE_H_
#define OCTOON_RENDER_SCENE_H_

#include <octoon/light/light.h>
#include <octoon/camera/camera.h>

#include <octoon/runtime/singleton.h>

namespace octoon::video
{
	class OCTOON_EXPORT RenderScene final
	{
		OctoonDeclareSingleton(RenderScene)
	public:
		RenderScene() noexcept;
		~RenderScene() noexcept;

		void addCamera(camera::Camera* camera) noexcept;
		void removeCamera(camera::Camera* camera) noexcept;
		const std::vector<camera::Camera*>& getCameraList() const noexcept;

		void addLight(light::Light* camera) noexcept;
		void removeLight(light::Light* camera) noexcept;
		const std::vector<light::Light*>& getLights() const noexcept;

		void addRenderObject(RenderObject* object) noexcept;
		void removeRenderObject(RenderObject* object) noexcept;
		const std::vector<RenderObject*>& getRenderObjects() const noexcept;

		void sortCameras() noexcept;
		void sortRenderObjects() noexcept;

	private:
		RenderScene(const RenderScene&) = delete;
		RenderScene& operator=(const RenderScene&) = delete;

	private:
		std::vector<light::Light*> lights_;
		std::vector<camera::Camera*> cameras_;
		std::vector<RenderObject*> renderables_;
	};
}

#endif