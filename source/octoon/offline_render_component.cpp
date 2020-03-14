#include <octoon/offline_render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineRenderComponent, GameComponent, "OfflineRender")

	OfflineRenderComponent::OfflineRenderComponent() noexcept
		: isSharedMaterial_(false)
	{
	}

	OfflineRenderComponent::OfflineRenderComponent(material::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(std::move(material), sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(const material::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(material, sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(material::Materials&& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(std::move(material), sharedMaterial);
	}

	OfflineRenderComponent::OfflineRenderComponent(const material::Materials& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(material, sharedMaterial);
	}

	OfflineRenderComponent::~OfflineRenderComponent() noexcept
	{
	}

	void
	OfflineRenderComponent::setMaterial(material::MaterialPtr&& material, std::size_t n, bool sharedMaterial) noexcept
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
	OfflineRenderComponent::setMaterial(const material::MaterialPtr& material, std::size_t n, bool sharedMaterial) noexcept
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

	const material::MaterialPtr&
	OfflineRenderComponent::getMaterial(std::size_t n) const noexcept
	{
		return materials_[n];
	}

	void
	OfflineRenderComponent::setMaterials(material::Materials&& material, bool sharedMaterial) noexcept
	{
		materials_ = std::move(material);
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	void
	OfflineRenderComponent::setMaterials(const material::Materials& material, bool sharedMaterial) noexcept
	{
		materials_ = material;
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	const material::Materials&
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
	OfflineRenderComponent::onMaterialReplace(const material::Materials&) noexcept
	{
	}
}