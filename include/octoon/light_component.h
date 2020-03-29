#ifndef OCTOON_LIGHT_COMPONENT_H_
#define OCTOON_LIGHT_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT LightComponent : public GameComponent
	{
		OctoonDeclareSubInterface(LightComponent, GameComponent)
	public:
		LightComponent() noexcept;
		virtual ~LightComponent() noexcept;

		virtual void setIntensity(float value) noexcept;
		virtual float getIntensity() const noexcept;

		virtual void setColor(const math::float3& value) noexcept;
		virtual const math::float3& getColor() const noexcept;

	private:
		LightComponent(const LightComponent&) = delete;
		LightComponent& operator=(const LightComponent&) = delete;

	protected:
		float intensity_;
		math::float3 color_;
	};
}

#endif