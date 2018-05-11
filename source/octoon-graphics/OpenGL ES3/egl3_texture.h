#ifndef OCTOON_EGL3_TEXTURE_H_
#define OCTOON_EGL3_TEXTURE_H_

#include "egl3_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL3Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(EGL3Texture, GraphicsTexture)
		public:
			EGL3Texture() noexcept;
			~EGL3Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const GraphicsTextureDesc& getGraphicsTextureDesc() const noexcept;

		private:
			static bool applyMipmapLimit(GLenum target, std::uint32_t min, std::uint32_t count) noexcept;
			static bool applySamplerWrap(GLenum target, GraphicsSamplerWrap wrap) noexcept;
			static bool applySamplerFilter(GLenum target, GraphicsSamplerFilter filter) noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

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