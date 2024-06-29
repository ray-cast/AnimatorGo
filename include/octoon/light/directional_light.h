#ifndef OCTOON_DIRECTIONAL_LIGHT_H_
#define OCTOON_DIRECTIONAL_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon
{
	class OCTOON_EXPORT DirectionalLight final : public Light
	{
		OctoonDeclareSubClass(DirectionalLight, Light)
	public:
		DirectionalLight() noexcept;
		virtual ~DirectionalLight() noexcept;

		void setShadowEnable(bool enable) noexcept;
		bool getShadowEnable() const noexcept;

		void setShadowBias(float bias) noexcept;
		float getShadowBias() const noexcept;

		void setShadowRadius(float radius) noexcept;
		float getShadowRadius() const noexcept;

		void setShadowMapSize(const math::uint2& size) noexcept;
		const math::uint2& getShadowMapSize() const noexcept;

		void setCamera(const std::shared_ptr<Camera>& camera) noexcept;
		const std::shared_ptr<Camera>& getCamera() const noexcept;

		std::shared_ptr<RenderObject> clone() const noexcept;

	private:
		void onMoveAfter() noexcept override;

	private:
		DirectionalLight(const DirectionalLight&) noexcept = delete;
		DirectionalLight& operator=(const DirectionalLight&) noexcept = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		math::uint2 shadowSize_;

		std::shared_ptr<Camera> shadowCamera_;
	};
}

#endif