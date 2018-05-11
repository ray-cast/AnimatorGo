#include "egl2_sampler.h"

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2Sampler, GraphicsSampler, "EGL2Sampler")

		EGL2Sampler::EGL2Sampler() noexcept
		{
		}

		EGL2Sampler::~EGL2Sampler() noexcept
		{
			this->close();
		}

		bool
		EGL2Sampler::setup(const GraphicsSamplerDesc& desc) except
		{
			_desc = desc;
			return true;
		}

		void
		EGL2Sampler::close() noexcept
		{
		}

		const GraphicsSamplerDesc&
		EGL2Sampler::getGraphicsSamplerDesc() const noexcept
		{
			return _desc;
		}

		void
		EGL2Sampler::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Sampler::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
