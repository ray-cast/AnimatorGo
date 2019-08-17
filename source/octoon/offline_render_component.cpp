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

	OfflineRenderComponent::~OfflineRenderComponent() noexcept
	{
	}

	void
	OfflineRenderComponent::setMaterial(model::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		if (material_ != material)
		{
			material_ = std::move(material);
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(material_);
		}
	}

	void
	OfflineRenderComponent::setMaterial(const model::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		if (material_ != material)
		{
			material_ = material;
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(material_);
		}
	}

	const model::MaterialPtr&
	OfflineRenderComponent::getMaterial() const noexcept
	{
		return material_;
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

	void
	OfflineRenderComponent::onPreRender() noexcept
	{
	}

	void
	OfflineRenderComponent::onPostRender() noexcept
	{
	}

	void
	OfflineRenderComponent::onMaterialReplace(const model::MaterialPtr&) noexcept
	{
	}
}