#ifndef OCTOON_GL32_STATE_H_
#define OCTOON_GL32_STATE_H_

#include "gl32_types.h"

namespace octoon 
{
	namespace hal
	{
		class GL32GraphicsState final : public RenderState
		{
			OctoonDeclareSubClass(GL32GraphicsState, RenderState)
		public:
			GL32GraphicsState() noexcept;
			~GL32GraphicsState() noexcept;

			bool setup(const RenderStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(RenderStateDesc& last) noexcept;

			const RenderStateDesc& getStateDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32GraphicsState(const GL32GraphicsState&) = delete;
			GL32GraphicsState& operator=(const GL32GraphicsState&) = delete;

		private:
			RenderStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif