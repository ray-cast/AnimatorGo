#ifndef OCTOON_GL33_INPUT_LAYOUT_H_
#define OCTOON_GL33_INPUT_LAYOUT_H_

#include "gl33_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL33InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(GL33InputLayout, GraphicsInputLayout)
		public:
			GL33InputLayout() noexcept;
			~GL33InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getInputLayoutDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33InputLayout(const GL33InputLayout&) noexcept = delete;
			GL33InputLayout& operator=(const GL33InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif