#include "gl32_input_layout.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32InputLayout, GraphicsInputLayout, "GL32InputLayout")

		GL32InputLayout::GL32InputLayout() noexcept
		{
		}

		GL32InputLayout::~GL32InputLayout() noexcept
		{
			this->close();
		}

		bool
		GL32InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		GL32InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		GL32InputLayout::getInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		GL32InputLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL32InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}