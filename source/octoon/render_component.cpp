#include <octoon/render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

	RenderComponent::RenderComponent() noexcept
		: isSharedMaterial_(false)
	{
	}

	RenderComponent::RenderComponent(video::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(std::move(material), sharedMaterial);
	}

	RenderComponent::RenderComponent(const video::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		this->setMaterial(material, sharedMaterial);
	}

	RenderComponent::~RenderComponent() noexcept
	{
	}

	void
	RenderComponent::setMaterial(video::MaterialPtr&& material, bool sharedMaterial) noexcept
	{
		if (material_ != material)
		{
			material_ = std::move(material);
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(material_);
		}
	}

	void
	RenderComponent::setMaterial(const video::MaterialPtr& material, bool sharedMaterial) noexcept
	{
		if (material_ != material)
		{
			material_ = material;
			isSharedMaterial_ = sharedMaterial;
			this->onMaterialReplace(material_);
		}
	}

	const video::MaterialPtr&
	RenderComponent::getMaterial() const noexcept
	{
		return material_;
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

	void
	RenderComponent::onPreRender(const video::Camera& camera) noexcept
	{
	}
	
	void
	RenderComponent::onPostRender(const video::Camera& camera) noexcept
	{
	}

	void
	RenderComponent::onMaterialReplace(const video::MaterialPtr&) noexcept
	{
	}
}