#ifndef OCTOON_OGL_CORE_TEXTURE_H_
#define OCTOON_OGL_CORE_TEXTURE_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLCoreTexture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(OGLCoreTexture, GraphicsTexture)
		public:
			OGLCoreTexture() noexcept;
			~OGLCoreTexture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

		private:
			bool applyMipmapLimit(std::uint32_t min, std::uint32_t count) noexcept;
			bool applySamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			bool applySamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;
			bool applySamplerAnis(GraphicsSamplerAnis anis) noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GLenum _target;
			GLuint _pbo;
			GLuint _upbo;
			GLuint _texture;
			GLuint _pboSize;
			GLuint _upboSize;
			GraphicsTextureDesc _textureDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif