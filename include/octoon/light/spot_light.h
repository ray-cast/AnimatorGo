#ifndef OCTOON_SPOT_LIGHT_H_
#define OCTOON_SPOT_LIGHT_H_

#include <octoon/light/light.h>
#include <octoon/hal/graphics_texture.h>

namespace octoon::light
{
	class OCTOON_EXPORT SpotLight final : public Light
	{
		OctoonDeclareSubClass(SpotLight, Light)
	public:
		SpotLight() noexcept;
		virtual ~SpotLight() noexcept;

		void setSpotInnerCone(float value) noexcept;
		void setSpotOuterCone(float value) noexcept;

		const math::float2& getSpotInnerCone() const noexcept;
		const math::float2& getSpotOuterCone() const noexcept;

		video::RenderObjectPtr clone() const noexcept;

	private:
		SpotLight(const SpotLight&) noexcept = delete;
		SpotLight& operator=(const SpotLight&) noexcept = delete;

	private:
		math::float2 _spotInnerCone;
		math::float2 _spotOuterCone;
	};
}

#endif