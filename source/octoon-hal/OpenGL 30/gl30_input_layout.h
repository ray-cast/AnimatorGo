#ifndef OCTOON_GL30_INPUT_LAYOUT_H_
#define OCTOON_GL30_INPUT_LAYOUT_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(GL30InputLayout, GraphicsInputLayout)
		public:
			GL30InputLayout() noexcept;
			~GL30InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getInputLayoutDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30InputLayout(const GL30InputLayout&) noexcept = delete;
			GL30InputLayout& operator=(const GL30InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif