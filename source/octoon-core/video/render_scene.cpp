#include <octoon/video/render_scene.h>

namespace octoon::video
{
	RenderScene::RenderScene() noexcept
		: mainCamera_(nullptr)
		, enableGlobalIllumination_(false)
	{
	}

	RenderScene::~RenderScene() noexcept
	{
	}

	void
	RenderScene::setMainCamera(camera::Camera* camera) noexcept
	{
		this->mainCamera_ = camera;
	}
	
	const camera::Camera*
	RenderScene::getMainCamera() const noexcept
	{
		return this->mainCamera_;
	}

	void
	RenderScene::setGlobalIllumination(bool enable) noexcept
	{
		if (this->enableGlobalIllumination_ != enable)
		{
			for (auto& it : cameras_)
				it->setDirty(true);
			for (auto& it : lights_)
				it->setDirty(true);
			for (auto& it : renderables_)
			{
				it->setDirty(true);
				for (auto& mat : it->getMaterials())
					mat->setDirty(true);
			}

			this->enableGlobalIllumination_ = enable;
		}
	}

	bool
	RenderScene::getGlobalIllumination() const noexcept
	{
		return this->enableGlobalIllumination_;
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
	RenderScene::getCameras() const noexcept
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
	RenderScene::addGeometry(geometry::Geometry* geometry) noexcept
	{
		assert(geometry);

		auto it = std::find(renderables_.begin(), renderables_.end(), geometry);
		if (it == renderables_.end())
			renderables_.push_back(geometry);
	}

	void
	RenderScene::removeGeometry(geometry::Geometry* geometry) noexcept
	{
		assert(geometry);

		auto it = std::find(renderables_.begin(), renderables_.end(), geometry);
		if (it != renderables_.end())
			renderables_.erase(it);
	}

	const std::vector<geometry::Geometry*>&
	RenderScene::getGeometries() const noexcept
	{
		return this->renderables_;
	}

	void
	RenderScene::addRenderObject(RenderObject* object) noexcept
	{
		assert(object);

		if (object->isA<camera::Camera>())
			this->addCamera(object->downcast<camera::Camera>());
		else if (object->isA<light::Light>())
			this->addLight(object->downcast<light::Light>());
		else if (object->isA<geometry::Geometry>())
			this->addGeometry(object->downcast<geometry::Geometry>());
	}

	void
	RenderScene::removeRenderObject(RenderObject* object) noexcept
	{
		assert(object);

		if (object->isA<camera::Camera>())
			this->removeCamera(object->downcast<camera::Camera>());
		else if (object->isA<light::Light>())
			this->removeLight(object->downcast<light::Light>());
		else if (object->isA<geometry::Geometry>())
			this->removeGeometry(object->downcast<geometry::Geometry>());
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
	RenderScene::sortGeometries() noexcept
	{
		std::sort(renderables_.begin(), renderables_.end(), [](const video::RenderObject* a, const video::RenderObject* b)
		{
			return a->getRenderOrder() < b->getRenderOrder();
		});
	}
}