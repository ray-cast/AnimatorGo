#include "light_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../rabbit_profile.h"
#include <octoon/perspective_camera_component.h>

namespace rabbit
{
	LightComponent::LightComponent() noexcept
	{
	}

	LightComponent::~LightComponent() noexcept
	{
	}

	void
	LightComponent::setActive(bool active) noexcept
	{
	}

	bool
	LightComponent::getActive() const noexcept
	{
		return true;
	}

	bool
	LightComponent::createLight(LightType type) noexcept
	{
		return true;
	}
}