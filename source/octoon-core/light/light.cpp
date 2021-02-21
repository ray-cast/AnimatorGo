#include <octoon/light/light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon
{
	OctoonImplementSubClass(Light, video::RenderObject, "Light")

	Light::Light() noexcept
		: lightIntensity_(1.0f)
		, lightRange_(10.0f)
		, lightColor_(math::float3::One)
	{
	}

	Light::~Light() noexcept
	{
	}

	void
	Light::setIntensity(float intensity) noexcept
	{
		if (lightIntensity_ != intensity)
		{
			lightIntensity_ = intensity;
			this->setDirty(true);
		}
	}

	void
	Light::setRange(float range) noexcept
	{
		if (lightRange_ != range)
		{
			lightRange_ = range;
			this->setDirty(true);
		}
	}

	void
	Light::setColor(const math::float3& color) noexcept
	{
		if (lightColor_ != color)
		{
			lightColor_ = color;
			this->setDirty(true);
		}
	}

	float
	Light::getIntensity() const noexcept
	{
		return lightIntensity_;
	}

	float
	Light::getRange() const noexcept
	{
		return lightRange_;
	}

	const math::float3&
	Light::getColor() const noexcept
	{
		return lightColor_;
	}

	void
	Light::onPreRender(const Camera& camera) noexcept
	{
	}

	void
	Light::onPostRender(const Camera& camera) noexcept
	{
	}

	std::shared_ptr<video::RenderObject>
	Light::clone() const noexcept
	{
		auto light = std::make_shared<Light>();
		light->setColor(this->getColor());
		light->setIntensity(this->getIntensity());
		light->setRange(this->getRange());
		light->setTransform(this->getTransform(), this->getTransformInverse());
		light->setBoundingBox(this->getBoundingBox());

		return light;
	}
}