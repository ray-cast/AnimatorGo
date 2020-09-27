#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/video_feature.h>
#include <octoon/video/renderer.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRendererComponent, RenderComponent, "MeshRenderer")

	MeshRendererComponent::MeshRendererComponent() noexcept
		: visible_(true)
		, renderOrder_(0)
		, globalIllumination_(false)
	{
	}

	MeshRendererComponent::MeshRendererComponent(material::MaterialPtr&& material) noexcept
		: MeshRendererComponent()
	{
		this->setMaterial(std::move(material));
	}

	MeshRendererComponent::MeshRendererComponent(const material::MaterialPtr& material) noexcept
		: MeshRendererComponent()
	{
		this->setMaterial(material);
	}

	MeshRendererComponent::MeshRendererComponent(material::Materials&& materials) noexcept
		: MeshRendererComponent()
	{
		this->setMaterials(std::move(materials));
	}

	MeshRendererComponent::MeshRendererComponent(const material::Materials& materials) noexcept
		: MeshRendererComponent()
	{
		this->setMaterials(materials);
	}

	MeshRendererComponent::~MeshRendererComponent() noexcept
	{
	}

	void
	MeshRendererComponent::setVisible(bool visible) noexcept
	{
		if (this->visible_ != visible)
		{
			this->geometry_->setVisible(visible);
			this->visible_ = visible;
		}
	}

	bool
	MeshRendererComponent::getVisible() const noexcept
	{
		return this->visible_;
	}

	void
	MeshRendererComponent::setRenderOrder(std::int32_t order) noexcept
	{
		if (this->renderOrder_ != order)
		{
			if (this->geometry_)
				this->geometry_->setRenderOrder(order);
			this->renderOrder_ = order;
		}
	}

	std::int32_t
	MeshRendererComponent::getRenderOrder() const noexcept
	{
		return this->renderOrder_;
	}

	void
	MeshRendererComponent::setGlobalIllumination(bool enable) noexcept
	{
		if (this->globalIllumination_ != enable)
		{
			if (this->geometry_)
				this->geometry_->setGlobalIllumination(enable);
			this->globalIllumination_ = enable;
		}
	}

	bool
	MeshRendererComponent::getGlobalIllumination() const noexcept
	{
		return this->globalIllumination_;
	}

	GameComponentPtr
	MeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshRendererComponent>();
		instance->setName(this->getName());

		if (!this->getMaterials().empty())
		{
			if (this->isSharedMaterial())
				instance->setMaterials(this->getMaterials());
			else
			{
				material::Materials materials;
				for (auto& it : this->getMaterials())
					materials.push_back(it->clone());
				instance->setMaterials(std::move(materials));
			}
		}

		return instance;
	}

	void
	MeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		
		this->onMaterialReplace(this->getMaterials());
		this->onMoveAfter();
		this->onLayerChangeAfter();

		this->sendMessage("octoon:mesh:get");
	}

	void
	MeshRendererComponent::onDeactivate() noexcept
	{
		if (geometry_)
		{
			this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(geometry_.get());
			this->geometry_.reset();
		}

		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
	}

	void
	MeshRendererComponent::onMeshReplace(const std::any& mesh_) noexcept
	{
		assert(mesh_.type() == typeid(mesh::MeshPtr));
		this->uploadMeshData(std::any_cast<mesh::MeshPtr>(mesh_));
	}

	void
	MeshRendererComponent::onMaterialReplace(const material::Materials& material) noexcept
	{
		this->uploadMaterialData(material);
	}

	void
	MeshRendererComponent::onMoveAfter() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		if (this->geometry_)
			this->geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	MeshRendererComponent::onLayerChangeAfter() noexcept
	{
		if (this->geometry_)
			this->geometry_->setLayer(this->getGameObject()->getLayer());
	}

	void
	MeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		if (mesh)
		{
			if (!geometry_)
			{
				geometry_ = std::make_shared<geometry::Geometry>();
				this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(geometry_.get());
			}

			geometry_->setOwnerListener(this);
			geometry_->setVisible(this->getVisible());
			geometry_->setGlobalIllumination(this->getGlobalIllumination());
			geometry_->setRenderOrder(this->getRenderOrder());
			geometry_->setBoundingBox(mesh->getBoundingBoxAll());
			geometry_->setMesh(mesh);
			geometry_->setMaterials(this->getMaterials());

			this->onMoveAfter();
			this->onLayerChangeAfter();
		}
		else
		{
			geometry_.reset();
		}
	}

	void
	MeshRendererComponent::uploadMaterialData(const material::Materials& materials) noexcept
	{
		if (geometry_)
			geometry_->setMaterials(materials);
	}
}