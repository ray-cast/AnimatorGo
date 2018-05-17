#ifndef OCTOON_GL32_INPUT_LAYOUT_H_
#define OCTOON_GL32_INPUT_LAYOUT_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(GL32InputLayout, GraphicsInputLayout)
		public:
			GL32InputLayout() noexcept;
			~GL32InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getInputLayoutDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL32InputLayout(const GL32InputLayout&) noexcept = delete;
			GL32InputLayout& operator=(const GL32InputLayout&) noexcept = delete;

		private:
			GraphicsInputLayoutDesc _inputLayoutDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif