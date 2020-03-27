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
	Light::setLightIntensity(float intensity) noexcept
	{
		_lightIntensity = intensity;
	}

	void
	Light::setLightRange(float range) noexcept
	{
		_lightRange = range;
	}

	void
	Light::setLightColor(const math::float3& color) noexcept
	{
		_lightColor = color;
	}

	float
	Light::getLightIntensity() const noexcept
	{
		return _lightIntensity;
	}

	float
	Light::getLightRange() const noexcept
	{
		return _lightRange;
	}

	const math::float3&
	Light::getLightColor() const noexcept
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

	video::RenderObjectPtr
	Light::clone() const noexcept
	{
		auto light = std::make_shared<Light>();
		light->setLightColor(this->getLightColor());
		light->setLightIntensity(this->getLightIntensity());
		light->setLightRange(this->getLightRange());
		light->setTransform(this->getTransform(), this->getTransformInverse());
		light->setBoundingBox(this->getBoundingBox());


		return light;
	}
}