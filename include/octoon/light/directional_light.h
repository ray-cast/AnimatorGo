#ifndef OCTOON_DIRECTIONAL_LIGHT_H_
#define OCTOON_DIRECTIONAL_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon::light
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

		void setCamera(const std::shared_ptr<camera::Camera>& camera) noexcept;
		const std::shared_ptr<camera::Camera>& getCamera() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

	private:
		DirectionalLight(const DirectionalLight&) noexcept = delete;
		DirectionalLight& operator=(const DirectionalLight&) noexcept = delete;

	private:
		bool shadowEnable_;

		float shadowBias_;
		float shadowRadius_;
		std::shared_ptr<camera::Camera> shadowCamera_;
	};
}

#endif