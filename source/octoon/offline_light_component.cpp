#include <octoon/offline_light_component.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineLightComponent, GameComponent, "OfflineLight")

	OfflineLightComponent::OfflineLightComponent() noexcept
		: intensity_(1.0f)
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
}