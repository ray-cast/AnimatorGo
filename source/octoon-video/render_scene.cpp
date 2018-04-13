#include <octoon/video/render_scene.h>
#include <octoon/video/camera.h>

namespace octoon
{
	namespace video
	{
		RenderScene::RenderScene() noexcept
		{
		}

		RenderScene::~RenderScene() noexcept
		{
		}

		void
		RenderScene::addCamera(Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it == cameras_.end())
				cameras_.push_back(camera);
		}

		void
		RenderScene::removeCamera(Camera* camera) noexcept
		{
			assert(camera);

			auto it = std::find(cameras_.begin(), cameras_.end(), camera);
			if (it != cameras_.end())
			{
				cameras_.erase(it);
				return;
			}
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

			if (object->is_instance_of<Camera>())
				this->addCamera(object->downcast<Camera>());
			else
				renderables_.push_back(object);
		}

		void
		RenderScene::removeRenderObject(RenderObject* object) noexcept
		{
			assert(object);

			if (object->is_instance_of<Camera>())
			{
				auto it = std::find(cameras_.begin(), cameras_.end(), object->downcast<Camera>());
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
	}
}