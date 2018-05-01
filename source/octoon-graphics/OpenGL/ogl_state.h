#ifndef OCTOON_OGL_CORE_STATE_H_
#define OCTOON_OGL_CORE_STATE_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLGraphicsState final : public GraphicsState
		{
			OctoonDeclareSubClass(OGLGraphicsState, RenderState)
		public:
			OGLGraphicsState() noexcept;
			~OGLGraphicsState() noexcept;

			bool setup(const GraphicsStateDesc& desc) noexcept;
			void close() noexcept;

			void apply(GraphicsStateDesc& last) noexcept;

			const GraphicsStateDesc& getGraphicsStateDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLGraphicsState(const OGLGraphicsState&) = delete;
			OGLGraphicsState& operator=(const OGLGraphicsState&) = delete;

		private:
			GraphicsStateDesc _stateDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif