#ifndef OCTOON_GL20_INPUT_LAYOUT_H_
#define OCTOON_GL20_INPUT_LAYOUT_H_

#include "gl20_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL20InputLayout final : public GraphicsInputLayout
		{
			OctoonDeclareSubClass(GL20InputLayout, GraphicsInputLayout)
		public:
			GL20InputLayout() noexcept;
			~GL20InputLayout() noexcept;

			bool setup(const GraphicsInputLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsInputLayoutDesc& getGraphicsInputLayoutDesc() const noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

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