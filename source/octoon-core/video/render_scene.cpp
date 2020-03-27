#include <octoon/video/render_scene.h>

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

		const std::vector<camera::Camera*>&
		RenderScene::getCameraList() const noexcept
		{
			return cameras_;
		}

		void
		RenderScene::addLight(light::Light* light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it == lights_.end())
				lights_.push_back(light);
		}

		void
		RenderScene::removeLight(light::Light* light) noexcept
		{
			assert(light);

			auto it = std::find(lights_.begin(), lights_.end(), light);
			if (it != lights_.end())
				lights_.erase(it);
		}

		const std::vector<light::Light*>&
		RenderScene::getLights() const noexcept
		{
			return lights_;
		}

		void
		RenderScene::addRenderObject(RenderObject* object) noexcept
		{
			assert(object);

			if (object->isA<camera::Camera>())
				this->addCamera(object->downcast<camera::Camera>());
			else if (object->isA<light::Light>())
				this->addLight(object->downcast<light::Light>());
			else
				renderables_.push_back(object);
		}

		void
		RenderScene::removeRenderObject(RenderObject* object) noexcept
		{
			assert(object);

			if (object->isA<camera::Camera>())
				this->removeCamera(object->downcast<camera::Camera>());
			else if (object->isA<light::Light>())
				this->removeLight(object->downcast<light::Light>());
			else
			{
				auto it = std::find(renderables_.begin(), renderables_.end(), object);
				if (it != renderables_.end())
					renderables_.erase(it);
			}
		}

		const std::vector<RenderObject*>&
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