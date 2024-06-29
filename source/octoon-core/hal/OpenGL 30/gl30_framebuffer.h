#ifndef OCTOON_GL30_FRAMEBUFFER_H_
#define OCTOON_GL30_FRAMEBUFFER_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(GL30FramebufferLayout, GraphicsFramebufferLayout)
		public:
			GL30FramebufferLayout() noexcept;
			~GL30FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL30FramebufferLayout(const GL30FramebufferLayout&) noexcept = delete;
			GL30FramebufferLayout& operator=(const GL30FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class GL30Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(GL30Framebuffer, GraphicsFramebuffer)
		public:
			GL30Framebuffer() noexcept;
			~GL30Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const std::uint64_t handle() const noexcept override;
			const GraphicsFramebufferDesc& getFramebufferDesc() const noexcept override;

		private:
			bool bindRenderTexture(std::shared_ptr<GraphicsTexture> target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL30Framebuffer(const GL30Framebuffer&) noexcept = delete;
			GL30Framebuffer& operator=(const GL30Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif