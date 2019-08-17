#include <octoon/offline_render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineRenderComponent, GameComponent, "OfflineRender")

	OfflineRenderComponent::OfflineRenderComponent() noexcept
		: isSharedMaterial_(false)
	{
	}

	OfflineRenderComponent::OfflineRenderComponent(model::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(std::move(material), sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(const model::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(material, sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(model::Materials&& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(std::move(material), sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(const model::Materials& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(material, sharedMaterial);
	}

	OfflineRenderComponent::~OfflineRenderComponent() noexcept
	{
	}

	void
	OfflineRenderComponent::setMaterial(model::MaterialPtr&& material, std::size_t n, bool sharedMaterial) noexcept
	{
		if (materials_.size() <= n)
			materials_.resize(n + 1);

		if (materials_[n] != material)
		{
			materials_[n] = std::move(material);
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(materials_);
		}
	}

	void
	OfflineRenderComponent::setMaterial(const model::MaterialPtr& material, std::size_t n, bool sharedMaterial) noexcept
	{
		if (materials_.size() <= n)
			materials_.resize(n + 1);

		if (materials_[n] != material)
		{
			materials_[n] = material;
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(materials_);
		}
	}

	const model::MaterialPtr&
	OfflineRenderComponent::getMaterial(std::size_t n) const noexcept
	{
		return materials_[n];
	}

	void
	OfflineRenderComponent::setMaterials(model::Materials&& material, bool sharedMaterial) noexcept
	{
		materials_ = std::move(material);
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	void
	OfflineRenderComponent::setMaterials(const model::Materials& material, bool sharedMaterial) noexcept
	{
		materials_ = material;
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	const model::Materials&
	OfflineRenderComponent::getMaterials() const noexcept
	{
		return materials_;
	}

	bool
	OfflineRenderComponent::isSharedMaterial() const noexcept
	{
		return isSharedMaterial_;
	}

	void
	OfflineRenderComponent::isSharedMaterial(bool sharedMaterial) noexcept
	{
		isSharedMaterial_ = sharedMaterial;
	}

	std::size_t
	OfflineRenderComponent::getNumMaterials() const noexcept
	{
		return materials_.size();
	}

	void
	OfflineRenderComponent::onPreRender() noexcept
	{
	}
	
	void
	OfflineRenderComponent::onPostRender() noexcept
	{
	}

	void
	OfflineRenderComponent::onMaterialReplace(const model::Materials&) noexcept
	{
	}
}