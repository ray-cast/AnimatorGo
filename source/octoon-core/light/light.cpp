#include <octoon/light/light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(Light, video::RenderObject, "Light")

	Light::Light() noexcept
		: _lightIntensity(1.0f)
		, _lightRange(10.0f)
		, _lightColor(math::float3::One)
	{
	}

	Light::~Light() noexcept
	{
	}

	void
	Light::setIntensity(float intensity) noexcept
	{
		_lightIntensity = intensity;
	}

	void
	Light::setRange(float range) noexcept
	{
		_lightRange = range;
	}

	void
	Light::setColor(const math::float3& color) noexcept
	{
		_lightColor = color;
	}

	float
	Light::getIntensity() const noexcept
	{
		return _lightIntensity;
	}

	float
	Light::getRange() const noexcept
	{
		return _lightRange;
	}

	const math::float3&
	Light::getColor() const noexcept
	{
		return _lightColor;
	}

	void
	Light::onPreRender(const camera::Camera& camera) noexcept
	{
	}

	void
	Light::onPostRender(const camera::Camera& camera) noexcept
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