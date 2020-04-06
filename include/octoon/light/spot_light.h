#ifndef OCTOON_SPOT_LIGHT_H_
#define OCTOON_SPOT_LIGHT_H_

#include <octoon/light/light.h>

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

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowRadius(float radius) noexcept;
		float getShadowRadius() const noexcept;

		void setShadowMapSize(const math::uint2& size) noexcept;
		const math::uint2& getShadowMapSize() const noexcept;

		void setCamera(const std::shared_ptr<camera::Camera>& camera) noexcept;
		const std::shared_ptr<camera::Camera>& getCamera() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		SpotLight(const SpotLight&) noexcept = delete;
		SpotLight& operator=(const SpotLight&) noexcept = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;

		math::uint2 shadowSize_;
		math::float2 innerCone_;
		math::float2 outerCone_;

		std::shared_ptr<camera::Camera> shadowCamera_;
	};
}

#endif