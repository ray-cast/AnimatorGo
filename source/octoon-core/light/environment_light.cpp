#include <octoon/light/environment_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(EnvironmentLight, Light, "EnvironmentLight")

	EnvironmentLight::EnvironmentLight() noexcept
	{
	}

	EnvironmentLight::~EnvironmentLight() noexcept
	{
	}

	void
	EnvironmentLight::setIrradiance(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		irradiance_ = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getIrradiance() const noexcept
	{
		return irradiance_;
	}

	void
	EnvironmentLight::setRadiance(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		radiance_ = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getRadiance() const noexcept
	{
		return radiance_;
	}

	std::shared_ptr<video::RenderObject>
	EnvironmentLight::clone() const noexcept
	{
		return std::make_shared<EnvironmentLight>();
	}
}