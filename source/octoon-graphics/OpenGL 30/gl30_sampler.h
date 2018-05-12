#ifndef OCTOON_GL30_SAMPLER_H_
#define OCTOON_GL30_SAMPLER_H_

#include "gl30_types.h"

namespace octoon 
{
	namespace graphics
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

			const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept;

		private:
			friend class GL30Device;
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