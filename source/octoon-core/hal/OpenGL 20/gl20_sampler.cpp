#include "gl20_sampler.h"

namespace octoon 
{
	namespace hal
	{
		OctoonImplementSubClass(GL20Sampler, GraphicsSampler, "GL20Sampler")

		GL20Sampler::GL20Sampler() noexcept
		{
		}

		GL20Sampler::~GL20Sampler() noexcept
		{
			this->close();
		}

		bool
		GL20Sampler::setup(const GraphicsSamplerDesc& desc) except
		{
			_desc = desc;
			return true;
		}

		void
		GL20Sampler::close() noexcept
		{
		}

		const GraphicsSamplerDesc&
		GL20Sampler::getSamplerDesc() const noexcept
		{
			return _desc;
		}

		void
		GL20Sampler::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20Sampler::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
