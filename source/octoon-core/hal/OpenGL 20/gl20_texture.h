#ifndef OCTOON_GL20_TEXTURE_H_
#define OCTOON_GL20_TEXTURE_H_

#include "gl20_types.h"

namespace octoon
{
	namespace hal
	{
		class GL20Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(GL20Texture, GraphicsTexture)
		public:
			GL20Texture() noexcept;
			~GL20Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept override;
			void unmap() noexcept override;

			void setSampler(const GraphicsSamplerPtr& sampler) noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const std::uint64_t handle() const noexcept override;
			const GraphicsTextureDesc& getTextureDesc() const noexcept override;

		private:
			static bool applySamplerWrap(GLenum target, SamplerWrap wrap) noexcept;
			static bool applySamplerFilter(GLenum target, SamplerFilter minFilter, SamplerFilter magFilter) noexcept;

		private:
			GLenum _target;
			GLuint _texture;
			GraphicsSamplerPtr _sampler;
			GraphicsTextureDesc _textureDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif