#ifndef OCTOON_OGL_FRAMEBUFFER_H_
#define OCTOON_OGL_FRAMEBUFFER_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLFramebufferLayout final : public GraphicsFramebufferLayout
		{
			OctoonDeclareSubClass(OGLFramebufferLayout, GraphicsFramebufferLayout)
		public:
			OGLFramebufferLayout() noexcept;
			~OGLFramebufferLayout() noexcept;

			bool setup(const GraphicsFramebufferLayoutDesc& framebufferDesc) noexcept;
			void close() noexcept;

			const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLFramebufferLayout(const OGLFramebufferLayout&) noexcept = delete;
			OGLFramebufferLayout& operator=(const OGLFramebufferLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferLayoutDesc _framebufferLayoutDesc;
		};

		class OGLFramebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(OGLFramebuffer, GraphicsFramebuffer)
		public:
			OGLFramebuffer() noexcept;
			~OGLFramebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLFramebuffer(const OGLFramebuffer&) noexcept = delete;
			OGLFramebuffer& operator=(const OGLFramebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif