#ifndef OCTOON_OGL_DEVICE_PROPERTY_H_
#define OCTOON_OGL_DEVICE_PROPERTY_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLDeviceProperty final : public GraphicsDeviceProperty
		{
		public:
			OGLDeviceProperty() noexcept;
			~OGLDeviceProperty() noexcept;

			bool setup() noexcept;
			void close() noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

			const GraphicsDeviceProperties& getGraphicsDeviceProperties() const noexcept;

		private:
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
			struct CreateParam
			{
				HWND hwnd;
				HDC hdc;
				HINSTANCE hinstance;
				HGLRC context;

				CreateParam()
					: hwnd(nullptr)
					, hdc(nullptr)
					, hinstance(nullptr)
					, context(nullptr)
				{
				}
			};
#elif defined(OCTOON_BUILD_PLATFORM_LINUX)
			struct CreateParam
			{
				Display* dpy;
				XVisualInfo* vi;
				GLXContext ctx;
				Colormap cmap;
				Window wnd;
				GLXFBConfig* config;

				CreateParam()
					: dpy(nullptr)
					, vi(nullptr)
					, ctx(nullptr)
					, cmap(0)
					, wnd(0)
					, config(0)
				{
				}
			};
#elif defined(OCTOON_BUILD_PLATFORM_APPLE)
			struct CreateParam
			{
				CGLContextObj ctx;
				CGLContextObj octx;

				CreateParam()
					: ctx(nullptr)
					, ctx(nullptr)
				{
				}
			};
#endif

		private:
			bool setupGLEnvironment(CreateParam& param) noexcept;
			void closeGLEnvironment(const CreateParam& param) noexcept;

			bool initDeviceProperties() noexcept;

			bool initTextureSupports() noexcept;
			bool initTextureDimSupports() noexcept;
			bool initVertexSupports() noexcept;
			bool initShaderSupports() noexcept;

		private:
			OGLDeviceProperty(const OGLDeviceProperty&) = delete;
			OGLDeviceProperty& operator=(const OGLDeviceProperty&) = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif