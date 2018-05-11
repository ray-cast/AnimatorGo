#include "egl2_input_layout.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2InputLayout, GraphicsInputLayout, "EGL2InputLayout")

		EGL2InputLayout::EGL2InputLayout() noexcept
		{
		}

		EGL2InputLayout::~EGL2InputLayout() noexcept
		{
			this->close();
		}

		bool
		EGL2InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		EGL2InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		EGL2InputLayout::getGraphicsInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		EGL2InputLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
