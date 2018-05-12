#ifndef OCTOON_OGL_TEXTURE_H_
#define OCTOON_OGL_TEXTURE_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLTexture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(OGLTexture, GraphicsTexture)
		public:
			OGLTexture() noexcept;
			~OGLTexture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

		private:
			bool applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept;
			bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
			bool applySamplerFilter(GLenum target, GraphicsSamplerFilter min, GraphicsSamplerFilter mag) noexcept;
			bool applySamplerAnis(GLenum target, GraphicsSamplerAnis anis) noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

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