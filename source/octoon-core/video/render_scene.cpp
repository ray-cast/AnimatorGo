#include <octoon/video/render_scene.h>

namespace octoon
{
	RenderScene::RenderScene() noexcept
		: mainCamera_(nullptr)
		, enableGlobalIllumination_(false)
		, dirty_(true)
	{
	}

	RenderScene::~RenderScene() noexcept
	{
	}

	void
	RenderScene::setMainCamera(Camera* camera) noexcept
	{
		this->mainCamera_ = camera;
	}
	
	const Camera*
	RenderScene::getMainCamera() const noexcept
	{
		return this->mainCamera_;
	}

	void
	RenderScene::setDirty(bool dirty) noexcept
	{
		dirty_ = dirty;

		for (auto& it : cameras_)
			it->setDirty(dirty);
		for (auto& it : lights_)
			it->setDirty(dirty);

		for (auto& it : renderables_)
		{
			it->setDirty(dirty);

			auto mesh = it->getMesh();
			if (mesh)
				mesh->setDirty(true);

			for (auto& material : it->getMaterials())
			{
				if (material)
					material->setDirty(true);
			}
		}
	}

	bool
	RenderScene::isDirty() const noexcept
	{
		if (dirty_)
			return true;

		for (auto& it : cameras_)
		{
			if (it->isDirty())
				return true;
		}

		for (auto& it : lights_)
		{
			if (it->isDirty())
				return true;
		}

		for (auto& it : renderables_)
		{
			if (it->isDirty())
				return true;

			auto mesh = it->getMesh();
			if (mesh)
			{
				if (mesh->isDirty())
					return true;
			}

			for (auto& material : it->getMaterials())
			{
				if (material)
				{
					if (material->isDirty())
						return true;
				}
			}
		}

		return false;
	}

	void
	RenderScene::setSceneDirty(bool dirty) noexcept
	{
		dirty_ = dirty;
	}

	bool
	RenderScene::isSceneDirty() const noexcept
	{
		return dirty_;
	}

	void
	RenderScene::setGlobalIllumination(bool enable) noexcept
	{
		if (this->enableGlobalIllumination_ != enable)
		{
			this->setDirty(true);

			this->enableGlobalIllumination_ = enable;
		}
	}

	bool
	RenderScene::getGlobalIllumination() const noexcept
	{
		return this->enableGlobalIllumination_;
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
			cameras_.erase(it);
	}

	const std::vector<Camera*>&
	RenderScene::getCameras() const noexcept
	{
		return cameras_;
	}

	void
	RenderScene::addLight(Light* light) noexcept
	{
		assert(light);

		auto it = std::find(lights_.begin(), lights_.end(), light);
		if (it == lights_.end())
			lights_.push_back(light);
	}

	void
	RenderScene::removeLight(Light* light) noexcept
	{
		assert(light);

		auto it = std::find(lights_.begin(), lights_.end(), light);
		if (it != lights_.end())
			lights_.erase(it);
	}

	const std::vector<Light*>&
	RenderScene::getLights() const noexcept
	{
		return lights_;
	}

	void
	RenderScene::addGeometry(Geometry* geometry) noexcept
	{
		assert(geometry);

		auto it = std::find(renderables_.begin(), renderables_.end(), geometry);
		if (it == renderables_.end())
			renderables_.push_back(geometry);
	}

	void
	RenderScene::removeGeometry(Geometry* geometry) noexcept
	{
		assert(geometry);

		auto it = std::find(renderables_.begin(), renderables_.end(), geometry);
		if (it != renderables_.end())
			renderables_.erase(it);
	}

	const std::vector<Geometry*>&
	RenderScene::getGeometries() const noexcept
	{
		return this->renderables_;
	}

	void
	RenderScene::addRenderObject(RenderObject* object) noexcept
	{
		assert(object);

		if (object->isA<Camera>())
			this->addCamera(object->downcast<Camera>());
		else if (object->isA<Light>())
			this->addLight(object->downcast<Light>());
		else if (object->isA<Geometry>())
			this->addGeometry(object->downcast<Geometry>());

		dirty_ = true;
	}

	void
	RenderScene::removeRenderObject(RenderObject* object) noexcept
	{
		assert(object);

		if (object->isA<Camera>())
			this->removeCamera(object->downcast<Camera>());
		else if (object->isA<Light>())
			this->removeLight(object->downcast<Light>());
		else if (object->isA<Geometry>())
			this->removeGeometry(object->downcast<Geometry>());

		dirty_ = true;
	}

	void
	RenderScene::sortCameras() noexcept
	{
		std::sort(cameras_.begin(), cameras_.end(), [](const Camera* a, const Camera* b)
		{
			return a->getRendererPriority() < b->getRendererPriority();
		});
	}

	void
	RenderScene::sortGeometries() noexcept
	{
		std::sort(renderables_.begin(), renderables_.end(), [](const RenderObject* a, const RenderObject* b)
		{
			return a->getRendererPriority() < b->getRendererPriority();
		});
	}
}