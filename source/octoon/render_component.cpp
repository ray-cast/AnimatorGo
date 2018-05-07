#include <octoon/render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

	RenderComponent::RenderComponent() noexcept
	{
	}

	RenderComponent::RenderComponent(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	RenderComponent::RenderComponent(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	RenderComponent::~RenderComponent() noexcept
	{
	}

	void
	RenderComponent::setMaterial(video::MaterialPtr&& material) noexcept
	{
		if (material_ != material)
		{
			this->onMaterialReplace(material);
			material_ = std::move(material);
		}
	}

	void
	RenderComponent::setMaterial(const video::MaterialPtr& material) noexcept
	{
		if (material_ != material)
		{
			this->onMaterialReplace(material);
			material_ = material;
		}
	}

	const video::MaterialPtr&
	RenderComponent::getMaterial() const noexcept
	{
		return material_;
	}

	void
	RenderComponent::setSharedMaterial(video::MaterialPtr&& material) noexcept
	{
		if (sharedMaterial_ != material)
		{
			this->onMaterialReplace(material);
			sharedMaterial_ = std::move(material);
		}
	}

	void
	RenderComponent::setSharedMaterial(const video::MaterialPtr& material) noexcept
	{
		if (sharedMaterial_ != material)
		{
			this->onMaterialReplace(material);
			sharedMaterial_ = material;
		}
	}

	const video::MaterialPtr&
	RenderComponent::getSharedMaterial() const noexcept
	{
		return sharedMaterial_;
	}

	void
	RenderComponent::onMaterialReplace(const video::MaterialPtr&) noexcept
	{
	}

	void
	RenderComponent::onSharedMaterialReplace(const video::MaterialPtr&) noexcept
	{
	}
}