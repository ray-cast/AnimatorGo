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

		void setSkyTexture(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getSkyTexture() const noexcept;

		void setSkyDiffuseTexture(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getSkyDiffuseTexture() const noexcept;

		void setSkySpecularTexture(const hal::GraphicsTexturePtr& texture) noexcept;
		const hal::GraphicsTexturePtr& getSkySpecularTexture() const noexcept;

		video::RenderObjectPtr clone() const noexcept;

	private:
		EnvironmentLight(const EnvironmentLight&) noexcept = delete;
		EnvironmentLight& operator=(const EnvironmentLight&) noexcept = delete;

	private:
		hal::GraphicsTexturePtr _skybox;
		hal::GraphicsTexturePtr _skyDiffuseIBL;
		hal::GraphicsTexturePtr _skySpecularIBL;
	};
}

#endif