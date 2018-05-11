#include "gl20_input_layout.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL20InputLayout, GraphicsInputLayout, "GL20InputLayout")

		GL20InputLayout::GL20InputLayout() noexcept
		{
		}

		GL20InputLayout::~GL20InputLayout() noexcept
		{
			this->close();
		}

		bool
		GL20InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		GL20InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		GL20InputLayout::getGraphicsInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		GL20InputLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL20InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
