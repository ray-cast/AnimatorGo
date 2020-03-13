#ifndef OCTOON_GL20_SAMPLER_H_
#define OCTOON_GL20_SAMPLER_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL20Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(GL20Sampler, GraphicsSampler)
		public:
			GL20Sampler() noexcept;
			~GL20Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			const GraphicsSamplerDesc& getSamplerDesc() const noexcept override;
		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GraphicsSamplerDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif