#ifndef OCTOON_GL30_SAMPLER_H_
#define OCTOON_GL30_SAMPLER_H_

#include "gl30_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL30Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(GL30Sampler, SamplerObject)
		public:
			GL30Sampler() noexcept;
			~GL30Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsSamplerDesc& getSamplerDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GLuint _sampler;
			GraphicsSamplerDesc _sampleDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif