#ifndef OCTOON_GL20_FRAMEBUFFER_H_
#define OCTOON_GL20_FRAMEBUFFER_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL20FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(GL20FramebufferLayout, GraphicsFramebufferLayout)
		public:
			GL20FramebufferLayout() noexcept;
			~GL20FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept override;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL20FramebufferLayout(const GL20FramebufferLayout&) noexcept = delete;
			GL20FramebufferLayout& operator=(const GL20FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class GL20Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(GL20Framebuffer, GraphicsFramebuffer)
		public:
			GL20Framebuffer() noexcept;
			~GL20Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const std::uint64_t handle() const noexcept override;

			const GraphicsFramebufferDesc& getFramebufferDesc() const noexcept override;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL20Framebuffer(const GL20Framebuffer&) noexcept = delete;
			GL20Framebuffer& operator=(const GL20Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif