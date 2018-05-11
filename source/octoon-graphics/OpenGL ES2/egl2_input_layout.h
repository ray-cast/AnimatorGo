#ifndef OCTOON_EGL2_INPUT_LAYOUT_H_
#define OCTOON_EGL2_INPUT_LAYOUT_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(EGL2InputLayout, GraphicsInputLayout)
		public:
			EGL2InputLayout() noexcept;
			~EGL2InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getGraphicsInputLayoutDesc() const noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2InputLayout(const EGL2InputLayout&) noexcept = delete;
			EGL2InputLayout& operator=(const EGL2InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif