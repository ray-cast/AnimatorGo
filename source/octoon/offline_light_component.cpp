#include <octoon/offline_light_component.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineLightComponent, GameComponent, "OfflineLight")

	OfflineLightComponent::OfflineLightComponent() noexcept
		: intensity_(1.0f)
		, color_(math::float3::One)
	{
	}

	OfflineLightComponent::~OfflineLightComponent() noexcept
	{
	}

	void
	OfflineLightComponent::setIntensity(float value) noexcept
	{
		this->intensity_ = value;
	}

	float
	OfflineLightComponent::getIntensity() const noexcept
	{
		return this->intensity_;
	}

	void 
	OfflineLightComponent::setColor(const math::float3& value) noexcept
	{
		color_ = value;
	}

	const math::float3&
	OfflineLightComponent::getColor() const noexcept
	{
		return color_;
	}
}