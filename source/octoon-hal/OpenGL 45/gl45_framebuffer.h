#ifndef OCTOON_GL33_CORE_FRAMEBUFFER_H_
#define OCTOON_GL33_CORE_FRAMEBUFFER_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45Framebuffer final : public GraphicsFramebuffer
		{
			OctoonDeclareSubClass(GL45Framebuffer, GraphicsFramebuffer)
		public:
			GL45Framebuffer() noexcept;
			~GL45Framebuffer() noexcept;

			bool setup(const GraphicsFramebufferDesc& framebufferDesc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsFramebufferDesc& getGraphicsFramebufferDesc() const noexcept override;

		private:
			bool bindRenderTexture(GraphicsTexturePtr target, GLenum attachment, GLint level, GLint layer) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL45Framebuffer(const GL45Framebuffer&) noexcept = delete;
			GL45Framebuffer& operator=(const GL45Framebuffer&) noexcept = delete;

		private:
			GLuint _fbo;

			GraphicsDeviceWeakPtr _device;
			GraphicsFramebufferDesc _framebufferDesc;
		};
	}
}

#endif