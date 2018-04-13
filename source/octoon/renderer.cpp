#include <octoon/renderer.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubInterface(Renderer, GameComponent, "FontRenderer")

	Renderer::Renderer() noexcept
	{
	}

	Renderer::Renderer(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	Renderer::Renderer(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	Renderer::~Renderer() noexcept
	{
	}

	void
	Renderer::setMaterial(video::MaterialPtr&& material) noexcept
	{
		material_ = std::move(material);
	}

	void
	Renderer::setMaterial(const video::MaterialPtr& material) noexcept
	{
		material_ = material;
	}

	const video::MaterialPtr&
	Renderer::getMaterial() const noexcept
	{
		return material_;
	}
}