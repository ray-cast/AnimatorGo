#ifndef OCTOON_ENVIRONMENT_LIGHT_H_
#define OCTOON_ENVIRONMENT_LIGHT_H_

#include <octoon/light/light.h>
#include <octoon/hal/graphics_texture.h>

namespace octoon::light
{
	class OCTOON_EXPORT EnvironmentLight final : public Light
	{
		OctoonDeclareSubClass(EnvironmentLight, Light)
	public:
		EnvironmentLight() noexcept;
		virtual ~EnvironmentLight() noexcept;

		void setIrradiance(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getIrradiance() const noexcept;

		void setRadiance(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getRadiance() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		EnvironmentLight(const EnvironmentLight&) noexcept = delete;
		EnvironmentLight& operator=(const EnvironmentLight&) noexcept = delete;

	private:
		hal::GraphicsTexturePtr radiance_;
		hal::GraphicsTexturePtr irradiance_;
	};
}

#endif