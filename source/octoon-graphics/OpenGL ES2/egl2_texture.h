#ifndef OCTOON_EGL2_TEXTURE_H_
#define OCTOON_EGL2_TEXTURE_H_

#include "egl2_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL2Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(EGL2Texture, GraphicsTexture)
		public:
			EGL2Texture() noexcept;
			~EGL2Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

		private:
			static bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
			static bool applySamplerFilter(GLenum target, GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GLenum _target;
			GLuint _texture;
			GraphicsTextureDesc _textureDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif