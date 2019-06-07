#ifndef OCTOON_GL33_TEXTURE_H_
#define OCTOON_GL33_TEXTURE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(GL33Texture, GraphicsTexture)
		public:
			GL33Texture() noexcept;
			~GL33Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getTextureDesc() const noexcept override;

		private:
			bool applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept;
			bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
			bool applySamplerFilter(GLenum target, GraphicsSamplerFilter min, GraphicsSamplerFilter mag) noexcept;
			bool applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GLenum _target;
			GLuint _pbo;
			GLuint _upbo;
			GLuint _texture;
			GLsizei _pboSize;
			GLsizei _upboSize;
			GraphicsTextureDesc _textureDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif