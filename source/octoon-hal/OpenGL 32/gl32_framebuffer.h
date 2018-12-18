#ifndef OCTOON_GL32_FRAMEBUFFER_H_
#define OCTOON_GL32_FRAMEBUFFER_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(GL32FramebufferLayout, GraphicsFramebufferLayout)
		public:
			GL32FramebufferLayout() noexcept;
			~GL32FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL32FramebufferLayout(const GL32FramebufferLayout&) noexcept = delete;
			GL32FramebufferLayout& operator=(const GL32FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class GL32Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(GL32Framebuffer, GraphicsFramebuffer)
		public:
			GL32Framebuffer() noexcept;
			~GL32Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept override;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL32Framebuffer(const GL32Framebuffer&) noexcept = delete;
			GL32Framebuffer& operator=(const GL32Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif