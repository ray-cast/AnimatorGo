#include "gl30_input_layout.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL30InputLayout, GraphicsInputLayout, "GL30InputLayout")

		GL30InputLayout::GL30InputLayout() noexcept
		{
		}

		GL30InputLayout::~GL30InputLayout() noexcept
		{
			this->close();
		}

		bool
		GL30InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		GL30InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		GL30InputLayout::getInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		GL30InputLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL30InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}