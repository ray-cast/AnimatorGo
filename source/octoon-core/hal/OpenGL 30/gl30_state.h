#ifndef OCTOON_GL30_STATE_H_
#define OCTOON_GL30_STATE_H_

#include "gl30_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL30GraphicsState final : public RenderState
		{
			OctoonDeclareSubClass(GL30GraphicsState, RenderState)
		public:
			GL30GraphicsState() noexcept;
			~GL30GraphicsState() noexcept;

			bool setup(const RenderStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(RenderStateDesc& last) noexcept;

			const RenderStateDesc& getStateDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL30GraphicsState(const GL30GraphicsState&) = delete;
			GL30GraphicsState& operator=(const GL30GraphicsState&) = delete;

		private:
			RenderStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif