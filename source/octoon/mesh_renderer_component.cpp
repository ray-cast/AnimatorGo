#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_pipeline.h>
#include <octoon/video/renderer.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRendererComponent, RenderComponent, "MeshRenderer")

	MeshRendererComponent::MeshRendererComponent() noexcept
		: visible_(true)
		, renderOrder_(0)
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
			for (auto& it : this->geometries_)
				it->setVisible(visible);

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
			for (auto& it : this->geometries_)
				it->setRenderOrder(order);

			this->renderOrder_ = order;
		}
	}

	std::int32_t
	MeshRendererComponent::getRenderOrder() const noexcept
	{
		return this->renderOrder_;
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
		geometries_.clear();
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
		for (auto& it : geometries_)
			it->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	MeshRendererComponent::onLayerChangeAfter() noexcept
	{
		auto layer = this->getGameObject()->getLayer();
		for (auto& it : geometries_)
			it->setLayer(layer);
	}

	void
	MeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		if (mesh)
		{
			geometries_.resize(mesh->getNumSubsets(), nullptr);

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto geometry_ = std::make_shared<geometry::Geometry>();
				geometry_->setActive(true);
				geometry_->setOwnerListener(this);
				geometry_->setVisible(this->getVisible());
				geometry_->setRenderOrder(this->getRenderOrder());
				geometry_->setBoundingBox(mesh->getBoundingBox(i));
				geometry_->setMesh(mesh);
				geometry_->setMeshSubset(i);

				geometries_[i] = std::move(geometry_);
			}

			this->onMoveAfter();
			this->onLayerChangeAfter();
			this->uploadMaterialData(this->getMaterials());
		}
		else
		{
			geometries_.clear();
		}
	}

	void
	MeshRendererComponent::uploadMaterialData(const material::Materials& materials) noexcept
	{
		for (std::size_t i = 0; i < geometries_.size(); i++)
		{
			if (i < materials.size())
				geometries_[i]->setMaterial(materials[i]);
			else
				geometries_[i]->setMaterial(materials.front());
		}
	}
}