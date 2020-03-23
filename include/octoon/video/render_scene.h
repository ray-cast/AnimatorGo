#ifndef OCTOON_RENDER_SCENE_H_
#define OCTOON_RENDER_SCENE_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderScene final
		{
			OctoonDeclareSingleton(RenderScene)
		public:
			RenderScene() noexcept;
			~RenderScene() noexcept;

			void addCamera(camera::Camera* camera) noexcept;
			void removeCamera(camera::Camera* camera) noexcept;
			const CameraRaws& getCameraList() const noexcept;

			void addRenderObject(RenderObject* object) noexcept;
			void removeRenderObject(RenderObject* object) noexcept;
			const RenderObjectRaws& getRenderObjects() const noexcept;

			void sortCameras() noexcept;
			void sortRenderObjects() noexcept;

		private:
			RenderScene(const RenderScene&) = delete;
			RenderScene& operator=(const RenderScene&) = delete;

		private:
			CameraRaws cameras_;
			RenderObjectRaws renderables_;
		};
	}
}

#endif