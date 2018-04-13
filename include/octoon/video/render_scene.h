#ifndef OCTOON_RENDER_SCENE_H_
#define OCTOON_RENDER_SCENE_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderScene final : public runtime::Singleton<RenderScene>
		{
		public:
			RenderScene() noexcept;
			~RenderScene() noexcept;

			void addCamera(Camera* camera) noexcept;
			void removeCamera(Camera* camera) noexcept;
			const CameraRaws& getCameraList() const noexcept;

			void addRenderObject(RenderObject* object) noexcept;
			void removeRenderObject(RenderObject* object) noexcept;
			const RenderObjectRaws& getRenderObjects() const noexcept;

		private:
			CameraRaws cameras_;
			RenderObjectRaws renderables_;
		};
	}
}

#endif