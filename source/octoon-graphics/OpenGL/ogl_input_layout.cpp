#include "ogl_input_layout.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLInputLayout, GraphicsInputLayout, "OGLInputLayout")

		OGLInputLayout::OGLInputLayout() noexcept
		{
		}

		OGLInputLayout::~OGLInputLayout() noexcept
		{
			this->close();
		}

		bool
		OGLInputLayout::setup(const GraphicsInputLayoutDesc& inputLayoutDesc) noexcept
		{
			assert(!inputLayoutDesc.getVertexLayouts().empty());
			assert(!inputLayoutDesc.getVertexBindings().empty());

			_inputLayoutDesc = inputLayoutDesc;
			return true;
		}

		void
		OGLInputLayout::close() noexcept
		{
		}

		const GraphicsInputLayoutDesc&
		OGLInputLayout::getGraphicsInputLayoutDesc() const noexcept
		{
			return _inputLayoutDesc;
		}

		void
		OGLInputLayout::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLInputLayout::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}