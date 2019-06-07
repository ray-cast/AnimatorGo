#ifndef OCTOON_GL33_CORE_SAMPLER_H_
#define OCTOON_GL33_CORE_SAMPLER_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(GL33Sampler, SamplerObject)
		public:
			GL33Sampler() noexcept;
			~GL33Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsSamplerDesc& getSamplerDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GLuint _sampler;
			GraphicsSamplerDesc _sampleDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif