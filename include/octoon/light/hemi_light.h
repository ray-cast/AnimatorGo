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

		void setShadowBias(float bias) noexcept;
		void setShadowRadius(float radius) noexcept;

		float getShadowBias() const noexcept;
		float getShadowRadius() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		DirectionalLight(const DirectionalLight&) noexcept = delete;
		DirectionalLight& operator=(const DirectionalLight&) noexcept = delete;

	private:
		float shadowBias_;
		float shadowRadius_;
	};
}

#endif