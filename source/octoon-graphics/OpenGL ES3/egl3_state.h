#ifndef OCTOON_EGL3_STATE_H_
#define OCTOON_EGL3_STATE_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3GraphicsState final : public GraphicsState
		{
			OctoonDeclareSubClass(EGL3GraphicsState, RenderState)
		public:
			EGL3GraphicsState() noexcept;
			~EGL3GraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getGraphicsStateDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3GraphicsState(const EGL3GraphicsState&) = delete;
			EGL3GraphicsState& operator=(const EGL3GraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif