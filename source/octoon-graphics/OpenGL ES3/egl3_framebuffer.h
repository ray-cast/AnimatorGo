#ifndef OCTOON_EGL3_FRAMEBUFFER_H_
#define OCTOON_EGL3_FRAMEBUFFER_H_

#include "egl3_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL3FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(EGL3FramebufferLayout, GraphicsFramebufferLayout)
		public:
			EGL3FramebufferLayout() noexcept;
			~EGL3FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3FramebufferLayout(const EGL3FramebufferLayout&) noexcept = delete;
			EGL3FramebufferLayout& operator=(const EGL3FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class EGL3Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(EGL3Framebuffer, GraphicsFramebuffer)
		public:
			EGL3Framebuffer() noexcept;
			~EGL3Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3Framebuffer(const EGL3Framebuffer&) noexcept = delete;
			EGL3Framebuffer& operator=(const EGL3Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif