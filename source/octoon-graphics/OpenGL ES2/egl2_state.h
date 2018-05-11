#ifndef OCTOON_EGL2_STATE_H_
#define OCTOON_EGL2_STATE_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2GraphicsState : public GraphicsState
		{
			OctoonDeclareSubClass(EGL2GraphicsState, GraphicsState)
		public:
			EGL2GraphicsState() noexcept;
			~EGL2GraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getGraphicsStateDesc() const noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2GraphicsState(const EGL2GraphicsState&) = delete;
			EGL2GraphicsState& operator=(const EGL2GraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif