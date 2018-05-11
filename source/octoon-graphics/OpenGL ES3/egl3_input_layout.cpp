#include "egl3_input_layout.h"

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL3InputLayout, GraphicsInputLayout, "EGL3InputLayout")

		EGL3InputLayout::EGL3InputLayout() noexcept
		{
		}

		EGL3InputLayout::~EGL3InputLayout() noexcept
		{
			this->close();
		}

		bool
		EGL3InputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		EGL3InputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		EGL3InputLayout::getGraphicsInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		EGL3InputLayout::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL3InputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}
