#ifndef OCTOON_GL30_TEXTURE_H_
#define OCTOON_GL30_TEXTURE_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(GL30Texture, GraphicsTexture)
		public:
			GL30Texture() noexcept;
			~GL30Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept override;
			void unmap() noexcept override;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const std::uint64_t handle() const noexcept override;
			const GraphicsTextureDesc& getTextureDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

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