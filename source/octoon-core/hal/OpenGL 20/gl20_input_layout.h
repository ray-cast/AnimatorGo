#ifndef OCTOON_GL20_INPUT_LAYOUT_H_
#define OCTOON_GL20_INPUT_LAYOUT_H_

#include "gl20_types.h"

namespace octoon
{
	namespace hal
	{
		class GL20InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(GL20InputLayout, GraphicsInputLayout)
		public:
			GL20InputLayout() noexcept;
			~GL20InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

			const GraphicsInputLayoutDesc& getInputLayoutDesc() const noexcept override;

		private:
			GL20InputLayout(const GL20InputLayout&) noexcept = delete;
			GL20InputLayout& operator=(const GL20InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif