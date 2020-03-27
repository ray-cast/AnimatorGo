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
	EnvironmentLight::setSkyTexture(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		_skybox = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getSkyTexture() const noexcept
	{
		return _skybox;
	}

	void
	EnvironmentLight::setSkyDiffuseTexture(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube);
		_skyDiffuseIBL = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getSkyDiffuseTexture() const noexcept
	{
		return _skyDiffuseIBL;
	}

	void
	EnvironmentLight::setSkySpecularTexture(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube);
		_skySpecularIBL = texture;
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getSkySpecularTexture() const noexcept
	{
		return _skySpecularIBL;
	}

	std::shared_ptr<video::RenderObject>
	EnvironmentLight::clone() const noexcept
	{
		return std::make_shared<EnvironmentLight>();
	}
}