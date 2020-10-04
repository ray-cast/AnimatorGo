#include <octoon/light/environment_light.h>

namespace octoon::light
{
	OctoonImplementSubClass(EnvironmentLight, Light, "EnvironmentLight")

	EnvironmentLight::EnvironmentLight() noexcept
		: offset_(0, 0)
	{
	}

	EnvironmentLight::~EnvironmentLight() noexcept
	{
	}

	void
	EnvironmentLight::setOffset(const math::float2& offset) noexcept
	{
		if (this->offset_ != offset)
		{
			this->setDirty(true);
			this->offset_ = offset;
		}
	}

	const math::float2&
	EnvironmentLight::getOffset() const noexcept
	{
		return this->offset_;
	}

	void
	EnvironmentLight::setEnvironmentMap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		environmentMap_ = texture;
		this->setDirty(true);
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getEnvironmentMap() const noexcept
	{
		return environmentMap_;
	}

	std::shared_ptr<video::RenderObject>
	EnvironmentLight::clone() const noexcept
	{
		return std::make_shared<EnvironmentLight>();
	}
}