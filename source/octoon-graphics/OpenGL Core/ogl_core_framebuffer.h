#ifndef OCTOON_OGL_CORE_FRAMEBUFFER_H_
#define OCTOON_OGL_CORE_FRAMEBUFFER_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLCoreFramebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(OGLCoreFramebuffer, GraphicsFramebuffer)
		public:
			OGLCoreFramebuffer() noexcept;
			~OGLCoreFramebuffer() noexcept;

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
			OGLCoreFramebuffer(const OGLCoreFramebuffer&) noexcept = delete;
			OGLCoreFramebuffer& operator=(const OGLCoreFramebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif