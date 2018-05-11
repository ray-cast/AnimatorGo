#ifndef OCTOON_EGL3_SAMPLER_H_
#define OCTOON_EGL3_SAMPLER_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(EGL3Sampler, SamplerObject)
		public:
			EGL3Sampler() noexcept;
			~EGL3Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GLuint _sampler;
			GraphicsSamplerDesc _sampleDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif