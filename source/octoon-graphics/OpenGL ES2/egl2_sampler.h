#ifndef OCTOON_EGL2_SAMPLER_H_
#define OCTOON_EGL2_SAMPLER_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2Sampler final : public GraphicsSampler
		{
			OctoonDeclareSubClass(EGL2Sampler, GraphicsSampler)
		public:
			EGL2Sampler() noexcept;
			~EGL2Sampler() noexcept;

			bool setup(const GraphicsSamplerDesc& desc) except;
			void close() noexcept;

			const GraphicsSamplerDesc& getGraphicsSamplerDesc() const noexcept;
		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GraphicsSamplerDesc _desc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif