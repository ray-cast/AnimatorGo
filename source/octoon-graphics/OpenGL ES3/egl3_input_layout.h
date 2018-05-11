#ifndef OCTOON_EGL3_INPUT_LAYOUT_H_
#define OCTOON_EGL3_INPUT_LAYOUT_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(EGL3InputLayout, GraphicsInputLayout)
		public:
			EGL3InputLayout() noexcept;
			~EGL3InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getGraphicsInputLayoutDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3InputLayout(const EGL3InputLayout&) noexcept = delete;
			EGL3InputLayout& operator=(const EGL3InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif