#ifndef OCTOON_OGL_INPUT_LAYOUT_H_
#define OCTOON_OGL_INPUT_LAYOUT_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLInputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(OGLInputLayout, GraphicsInputLayout)
		public:
			OGLInputLayout() noexcept;
			~OGLInputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getGraphicsInputLayoutDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLInputLayout(const OGLInputLayout&) noexcept = delete;
			OGLInputLayout& operator=(const OGLInputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif