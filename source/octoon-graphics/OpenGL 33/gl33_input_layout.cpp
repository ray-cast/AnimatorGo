#include "gl33_input_layout.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL33InputLayout, GraphicsInputLayout, "GL33InputLayout")

		GL33InputLayout::GL33InputLayout() noexcept
		{
		}

		GL33InputLayout::~GL33InputLayout() noexcept
		{
			this->close();
		}

		bool
		GL33InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			assert(!inputLayoutDesc.getVertexLayouts().empty());
			assert(!inputLayoutDesc.getVertexBindings().empty());

			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		GL33InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		GL33InputLayout::getInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		GL33InputLayout::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}