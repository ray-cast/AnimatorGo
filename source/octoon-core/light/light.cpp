#include <octoon/light/light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon
{
	OctoonImplementSubClass(Light, RenderObject, "Light")

	Light::Light() noexcept
		: intensity_(1.0f)
		, size_(1.0f)
		, range_(10.0f)
		, color_(math::float3::One)
	{
	}

	Light::~Light() noexcept
	{
	}

	void
	Light::setIntensity(float intensity) noexcept
	{
		if (intensity_ != intensity)
		{
			intensity_ = intensity;
			this->setDirty(true);
		}
	}

	void
	Light::setSize(float size) noexcept
	{
		if (size_ != size)
		{
			size_ = size;
			this->setDirty(true);
		}
	}

	void
	Light::setRange(float range) noexcept
	{
		if (range_ != range)
		{
			range_ = range;
			this->setDirty(true);
		}
	}

	void
	Light::setColor(const math::float3& color) noexcept
	{
		if (color_ != color)
		{
			color_ = color;
			this->setDirty(true);
		}
	}

	float
	Light::getIntensity() const noexcept
	{
		return intensity_;
	}

	float
	Light::getSize() const noexcept
	{
		return size_;
	}

	float
	Light::getRange() const noexcept
	{
		return range_;
	}

	const math::float3&
	Light::getColor() const noexcept
	{
		return color_;
	}

	void
	Light::onPreRender(const Camera& camera) noexcept
	{
	}

	void
	Light::onPostRender(const Camera& camera) noexcept
	{
	}

	std::shared_ptr<RenderObject>
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