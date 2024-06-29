#ifndef OCTOON_GL33_CORE_STATE_H_
#define OCTOON_GL33_CORE_STATE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33GraphicsState final : public RenderState
		{
			OctoonDeclareSubClass(GL33GraphicsState, RenderState)
		public:
			GL33GraphicsState() noexcept;
			~GL33GraphicsState() noexcept;

			bool setup(const RenderStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(RenderStateDesc& last) noexcept;

			const RenderStateDesc& getStateDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL33GraphicsState(const GL33GraphicsState&) = delete;
			GL33GraphicsState& operator=(const GL33GraphicsState&) = delete;

		private:
			RenderStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif