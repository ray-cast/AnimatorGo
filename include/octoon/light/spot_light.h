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

		void setInnerCone(float value) noexcept;
		void setOuterCone(float value) noexcept;

		const math::float2& getInnerCone() const noexcept;
		const math::float2& getOuterCone() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		SpotLight(const SpotLight&) noexcept = delete;
		SpotLight& operator=(const SpotLight&) noexcept = delete;

	private:
		math::float2 _spotInnerCone;
		math::float2 _spotOuterCone;
	};
}

#endif