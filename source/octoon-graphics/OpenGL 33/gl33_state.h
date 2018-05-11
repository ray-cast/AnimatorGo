#ifndef OCTOON_GL33_CORE_STATE_H_
#define OCTOON_GL33_CORE_STATE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL33GraphicsState final : public GraphicsState
		{
			OctoonDeclareSubClass(GL33GraphicsState, RenderState)
		public:
			GL33GraphicsState() noexcept;
			~GL33GraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getGraphicsStateDesc() const noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL33GraphicsState(const GL33GraphicsState&) = delete;
			GL33GraphicsState& operator=(const GL33GraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif