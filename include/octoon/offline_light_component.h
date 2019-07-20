#ifndef OCTOON_OFFLINE_LIGHT_COMPONENT_H_
#define OCTOON_OFFLINE_LIGHT_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT OfflineLightComponent : public GameComponent
	{
		OctoonDeclareSubInterface(OfflineLightComponent, GameComponent)
	public:
		OfflineLightComponent() noexcept;
		virtual ~OfflineLightComponent() noexcept;

		virtual void setIntensity(float value) noexcept;
		virtual float getIntensity() const noexcept;

	private:
		OfflineLightComponent(const OfflineLightComponent&) = delete;
		OfflineLightComponent& operator=(const OfflineLightComponent&) = delete;

	private:
		float intensity_;
	};
}

#endif