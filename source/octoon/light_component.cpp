#include <octoon/light_component.h>

namespace octoon
{
	OctoonImplementSubInterface(LightComponent, GameComponent, "LightComponent")

	LightComponent::LightComponent() noexcept
		: intensity_(1.0f)
		, color_(math::float3::One)
		, size_(1.0f)
	{
	}

	LightComponent::~LightComponent() noexcept
	{
	}

	void
	LightComponent::setIntensity(float value) noexcept
	{
		this->intensity_ = value;
	}

	float
	LightComponent::getIntensity() const noexcept
	{
		return this->intensity_;
	}

	void
	LightComponent::setSize(float value) noexcept
	{
		size_ = value;
	}

	float
	LightComponent::getSize() const noexcept
	{
		return size_;
	}

	void 
	LightComponent::setColor(const math::float3& value) noexcept
	{
		color_ = value;
	}

	const math::float3&
	LightComponent::getColor() const noexcept
	{
		return color_;
	}
}