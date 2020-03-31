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

		void setEnvironmentMap(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getEnvironmentMap() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		EnvironmentLight(const EnvironmentLight&) noexcept = delete;
		EnvironmentLight& operator=(const EnvironmentLight&) noexcept = delete;

	private:
		hal::GraphicsTexturePtr radiance_;
		hal::GraphicsTexturePtr environmentMap_;
	};
}

#endif