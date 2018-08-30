#ifndef OCTOON_GL32_TEXTURE_H_
#define OCTOON_GL32_TEXTURE_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(GL32Texture, GraphicsTexture)
		public:
			GL32Texture() noexcept;
			~GL32Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getTextureDesc() const noexcept override;

		private:
			static bool applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept;
			static bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
			static bool applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GLenum _target;
			GLuint _texture;
			GLuint _pbo;
			GLsizei _pboSize;
			GraphicsTextureDesc _textureDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif