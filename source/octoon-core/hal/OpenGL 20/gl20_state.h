#ifndef OCTOON_GL20_STATE_H_
#define OCTOON_GL20_STATE_H_

#include "gl20_types.h"

namespace octoon
{
	namespace hal
	{
		class GL20GraphicsState : public RenderState
		{
			OctoonDeclareSubClass(GL20GraphicsState, RenderState)
		public:
			GL20GraphicsState() noexcept;
			~GL20GraphicsState() noexcept;

			bool setup(const RenderStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(RenderStateDesc& last) noexcept;

			const RenderStateDesc& getStateDesc() const noexcept override;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL20GraphicsState(const GL20GraphicsState&) = delete;
			GL20GraphicsState& operator=(const GL20GraphicsState&) = delete;

		private:
			RenderStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif