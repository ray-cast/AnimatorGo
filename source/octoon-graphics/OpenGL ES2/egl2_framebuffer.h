#ifndef OCTOON_EGL2_FRAMEBUFFER_H_
#define OCTOON_EGL2_FRAMEBUFFER_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2FramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(EGL2FramebufferLayout, GraphicsFramebufferLayout)
		public:
			EGL2FramebufferLayout() noexcept;
			~EGL2FramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2FramebufferLayout(const EGL2FramebufferLayout&) noexcept = delete;
			EGL2FramebufferLayout& operator=(const EGL2FramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class EGL2Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(EGL2Framebuffer, GraphicsFramebuffer)
		public:
			EGL2Framebuffer() noexcept;
			~EGL2Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2Framebuffer(const EGL2Framebuffer&) noexcept = delete;
			EGL2Framebuffer& operator=(const EGL2Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif