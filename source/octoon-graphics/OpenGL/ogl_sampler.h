#ifndef OCTOON_OGL_CORE_SAMPLER_H_
#define OCTOON_OGL_CORE_SAMPLER_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLSampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(OGLSampler, SamplerObject)
		public:
			OGLSampler() noexcept;
			~OGLSampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GLuint _sampler;
			GraphicsSamplerDesc _sampleDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif