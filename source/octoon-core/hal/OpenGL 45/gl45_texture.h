#ifndef OCTOON_GL45_CORE_TEXTURE_H_
#define OCTOON_GL45_CORE_TEXTURE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45Texture final : public GraphicsTexture
		{
			OctoonDeclareSubClass(GL45Texture, GraphicsTexture)
		public:
			GL45Texture() noexcept;
			~GL45Texture() noexcept;

			bool setup(const GraphicsTextureDesc& textureDesc) noexcept;
			void close() noexcept;

			bool map(std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h, std::uint32_t mipLevel, void** data) noexcept;
			void unmap() noexcept;

			GLenum getTarget() const noexcept;
			GLuint getInstanceID() const noexcept;

			const std::uint64_t handle() const noexcept override;
			const GraphicsTextureDesc& getTextureDesc() const noexcept override;

		private:
			bool applyMipmapLimit(std::uint32_t min, std::uint32_t count) noexcept;
			bool applySamplerWrap(GraphicsSamplerWrap wrap) noexcept;
			bool applySamplerFilter(GraphicsSamplerFilter minFilter, GraphicsSamplerFilter magFilter) noexcept;
			bool applySamplerAnis(GraphicsSamplerAnis anis) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

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