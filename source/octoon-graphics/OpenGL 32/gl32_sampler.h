#ifndef OCTOON_GL32_SAMPLER_H_
#define OCTOON_GL32_SAMPLER_H_

#include "gl32_types.h"

namespace octoon 
{
	namespace graphics
	{
		class GL32Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(GL32Sampler, SamplerObject)
		public:
			GL32Sampler() noexcept;
			~GL32Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			GLuint getInstanceID() noexcept;

			const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GLuint _sampler;
			GraphicsSamplerDesc _sampleDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif