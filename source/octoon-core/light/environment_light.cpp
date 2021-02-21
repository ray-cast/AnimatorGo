#include <octoon/light/environment_light.h>

namespace octoon
{
	OctoonImplementSubClass(EnvironmentLight, Light, "EnvironmentLight")

	EnvironmentLight::EnvironmentLight() noexcept
		: offset_(0, 0)
		, showBackground_(true)
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
			this->offset_ = offset;
			this->setDirty(true);
		}
	}

	const math::float2&
	EnvironmentLight::getOffset() const noexcept
	{
		return this->offset_;
	}

	void
	EnvironmentLight::setShowBackground(bool show) noexcept
	{
		if (this->showBackground_ != show)
		{
			this->showBackground_ = show;
			this->setDirty(true);
		}
	}

	bool
	EnvironmentLight::getShowBackground() const noexcept
	{
		return this->showBackground_;
	}

	void
	EnvironmentLight::setBackgroundMap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (backgroundMap_ != texture)
		{
			backgroundMap_ = texture;
			this->setDirty(true);
		}
	}

	const hal::GraphicsTexturePtr&
	EnvironmentLight::getBackgroundMap() const noexcept
	{
		return this->backgroundMap_;
	}

	void
	EnvironmentLight::setEnvironmentMap(const hal::GraphicsTexturePtr& texture) noexcept
	{
		assert(!texture || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Cube || texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2D);
		if (environmentMap_ != texture)
		{
			environmentMap_ = texture;
			this->setDirty(true);
		}
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