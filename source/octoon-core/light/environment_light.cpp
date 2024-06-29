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
	EnvironmentLight::setBackgroundMap(const std::shared_ptr<Texture>& texture) noexcept
	{
		assert(!texture || (texture->width() > 0 && texture->height() > 0));
		if (backgroundMap_ != texture)
		{
			backgroundMap_ = texture;
			this->setDirty(true);
		}
	}

	const std::shared_ptr<Texture>&
	EnvironmentLight::getBackgroundMap() const noexcept
	{
		return this->backgroundMap_;
	}

	void
	EnvironmentLight::setEnvironmentMap(const std::shared_ptr<Texture>& texture) noexcept
	{
		assert(!texture || (texture->width() > 0 && texture->height() > 0));
		if (environmentMap_ != texture)
		{
			environmentMap_ = texture;
			this->setDirty(true);
		}
	}

	const std::shared_ptr<Texture>&
	EnvironmentLight::getEnvironmentMap() const noexcept
	{
		return environmentMap_;
	}

	std::shared_ptr<RenderObject>
	EnvironmentLight::clone() const noexcept
	{
		return std::make_shared<EnvironmentLight>();
	}
}