#include <octoon/video/render_scene.h>
#include <octoon/camera/camera.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSingleton(RenderScene)

		RenderScene::RenderScene() noexcept
		{
		}

		RenderScene::~RenderScene() noexcept
		{
		}

		void
		RenderScene::addCamera(camera::Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it == cameras_.end())
				cameras_.push_back(camera);
		}

		void
		RenderScene::removeCamera(camera::Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it != cameras_.end())
				cameras_.erase(it);
		}

		const CameraRaws&
		RenderScene::getCameraList() const noexcept
		{
			return cameras_;
		}

		void
		RenderScene::addRenderObject(RenderObject* object) noexcept
		{
			assert(object);

			if (object->isA<camera::Camera>())
				this->addCamera(object->downcast<camera::Camera>());
			else
				renderables_.push_back(object);
		}

		void
		RenderScene::removeRenderObject(RenderObject* object) noexcept
		{
			assert(object);

			if (object->isA<camera::Camera>())
			{
				auto it = std::find(cameras_.begin(), cameras_.end(), object->downcast<camera::Camera>());
				if (it != cameras_.end())
					cameras_.erase(it);
			}
			else
			{
				auto it = std::find(renderables_.begin(), renderables_.end(), object);
				if (it != renderables_.end())
					renderables_.erase(it);
			}
		}

		const RenderObjectRaws&
		RenderScene::getRenderObjects() const noexcept
		{
			return renderables_;
		}

		void
		RenderScene::sortCameras() noexcept
		{
			std::sort(cameras_.begin(), cameras_.end(), [](const camera::Camera* a, const camera::Camera* b)
			{
				return a->getRenderOrder() < b->getRenderOrder();
			});
		}

		void
		RenderScene::sortRenderObjects() noexcept
		{
			std::sort(renderables_.begin(), renderables_.end(), [](const video::RenderObject* a, const video::RenderObject* b)
			{
				return a->getRenderOrder() < b->getRenderOrder();
			});
		}
	}
}