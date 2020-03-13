#ifndef OCTOON_GL30_STATE_H_
#define OCTOON_GL30_STATE_H_

#include "gl30_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL30GraphicsState final : public GraphicsState
		{
			OctoonDeclareSubClass(GL30GraphicsState, RenderState)
		public:
			GL30GraphicsState() noexcept;
			~GL30GraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getStateDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30GraphicsState(const GL30GraphicsState&) = delete;
			GL30GraphicsState& operator=(const GL30GraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif