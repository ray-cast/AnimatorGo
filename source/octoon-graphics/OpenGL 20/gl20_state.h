#ifndef OCTOON_GL20_STATE_H_
#define OCTOON_GL20_STATE_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace graphics
	{
		class GL20GraphicsState : public GraphicsState
		{
			OctoonDeclareSubClass(GL20GraphicsState, GraphicsState)
		public:
			GL20GraphicsState() noexcept;
			~GL20GraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getGraphicsStateDesc() const noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL20GraphicsState(const GL20GraphicsState&) = delete;
			GL20GraphicsState& operator=(const GL20GraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif