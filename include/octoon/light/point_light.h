#ifndef OCTOON_POINT_LIGHT_H_
#define OCTOON_POINT_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon
{
	class OCTOON_EXPORT PointLight final : public Light
	{
		OctoonDeclareSubClass(PointLight, Light)
	public:
		PointLight() noexcept;
		virtual ~PointLight() noexcept;

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowRadius(float radius) noexcept;
		float getShadowRadius() const noexcept;

		void setShadowMapSize(const math::uint2& size) noexcept;
		const math::uint2& getShadowMapSize() const noexcept;

		std::shared_ptr<RenderObject> clone() const noexcept;

		const std::shared_ptr<Camera>& getCamera() const noexcept;

	private:
		PointLight(const PointLight&) noexcept = delete;
		PointLight& operator=(const PointLight&) noexcept = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		math::uint2 shadowSize_;
		std::shared_ptr<Camera> shadowCamera_;
	};
}

#endif