#include <octoon/render_component.h>

namespace octoon
{
	OctoonImplementSubInterface(RenderComponent, GameComponent, "RenderComponent")

	RenderComponent::RenderComponent() noexcept
		: isSharedMaterial_(false)
	{
	}

	RenderComponent::RenderComponent(video::MaterialPtr&& material, bool sharedMesh) noexcept
	{
		this->setMaterial(std::move(material), sharedMesh);
	}

	RenderComponent::RenderComponent(const video::MaterialPtr& material, bool sharedMesh) noexcept
	{
		this->setMaterial(material, sharedMesh);
	}

	RenderComponent::~RenderComponent() noexcept
	{
	}

	void
	RenderComponent::setMaterial(video::MaterialPtr&& material, bool sharedMesh) noexcept
	{
		if (material_ != material)
		{
			material_ = std::move(material);
			isSharedMaterial_ = sharedMesh;
			this->onMaterialReplace(material);
		}
	}

	void
	RenderComponent::setMaterial(const video::MaterialPtr& material, bool sharedMesh) noexcept
	{
		if (material_ != material)
		{
			material_ = material;
			isSharedMaterial_ = sharedMesh;
			this->onMaterialReplace(material);
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
	RenderComponent::onMaterialReplace(const video::MaterialPtr&) noexcept
	{
	}
}