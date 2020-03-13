#ifndef OCTOON_GL33_FRAMEBUFFER_H_
#define OCTOON_GL33_FRAMEBUFFER_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(GL33FramebufferLayout, GraphicsFramebufferLayout)
		public:
			GL33FramebufferLayout() noexcept;
			~GL33FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33FramebufferLayout(const GL33FramebufferLayout&) noexcept = delete;
			GL33FramebufferLayout& operator=(const GL33FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class GL33Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(GL33Framebuffer, GraphicsFramebuffer)
		public:
			GL33Framebuffer() noexcept;
			~GL33Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const std::uint64_t handle() const noexcept override;
			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept override;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33Framebuffer(const GL33Framebuffer&) noexcept = delete;
			GL33Framebuffer& operator=(const GL33Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif