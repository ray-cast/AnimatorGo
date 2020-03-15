#include <octoon/render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

	RenderComponent::RenderComponent() noexcept
		: isSharedMaterial_(false)
	{
	}

	RenderComponent::RenderComponent(material::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(std::move(material), sharedMaterial);
	}

	RenderComponent::RenderComponent(const material::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(material, sharedMaterial);
	}

	RenderComponent::RenderComponent(material::Materials&& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(std::move(material), sharedMaterial);
	}

	RenderComponent::RenderComponent(const material::Materials& material, bool sharedMaterial) noexcept
	{
		this->setMaterials(material, sharedMaterial);
	}

	RenderComponent::~RenderComponent() noexcept
	{
	}

	void
	RenderComponent::setMaterial(material::MaterialPtr&& material, std::size_t n, bool sharedMaterial) noexcept
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
	RenderComponent::setMaterial(const material::MaterialPtr& material, std::size_t n, bool sharedMaterial) noexcept
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
	RenderComponent::getMaterial(std::size_t n) const noexcept
	{
		return materials_[n];
	}

	void
	RenderComponent::setMaterials(material::Materials&& material, bool sharedMaterial) noexcept
	{
		materials_ = std::move(material);
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	void
	RenderComponent::setMaterials(const material::Materials& material, bool sharedMaterial) noexcept
	{
		materials_ = material;
		isSharedMaterial_ = sharedMaterial;
		this->onMaterialReplace(materials_);
	}

	const material::Materials&
	RenderComponent::getMaterials() const noexcept
	{
		return materials_;
	}

	bool
	RenderComponent::isSharedMaterial() const noexcept
	{
		return isSharedMaterial_;
	}

	void
	RenderComponent::isSharedMaterial(bool sharedMaterial) noexcept
	{
		isSharedMaterial_ = sharedMaterial;
	}

	std::size_t
	RenderComponent::getNumMaterials() const noexcept
	{
		return materials_.size();
	}

	void
	RenderComponent::onPreRender(const camera::Camera& camera) noexcept
	{
	}
	
	void
	RenderComponent::onPostRender(const camera::Camera& camera) noexcept
	{
	}

	void
	RenderComponent::onMaterialReplace(const material::Materials&) noexcept
	{
	}
}