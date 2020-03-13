#ifndef OCTOON_OGL_DEVICE_PROPERTY_H_
#define OCTOON_OGL_DEVICE_PROPERTY_H_

#include "ogl_basic.h"

namespace octoon
{
	namespace hal
	{
		class OGLDeviceProperty : public GraphicsDeviceProperty
		{
		public:
			OGLDeviceProperty() noexcept;
			~OGLDeviceProperty() noexcept;

			bool setup(const GraphicsDeviceDesc& deviceDesc) noexcept;
			void close() noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

			const GraphicsDeviceProperties& getDeviceProperties() const noexcept;

		protected:
			virtual bool initDeviceProperties() noexcept = 0;

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
#else
			struct CreateParam
			{
				EGLDisplay display;
				EGLContext context;
				EGLSurface surface;

				CreateParam()
					: display(nullptr)
					, context(nullptr)
					, surface(nullptr)
				{
				}
			};
#endif

		private:
			void setupGLEnvironment(CreateParam& param) noexcept(false);
			void closeGLEnvironment(CreateParam& param) noexcept;

		private:
			OGLDeviceProperty(const OGLDeviceProperty&) = delete;
			OGLDeviceProperty& operator=(const OGLDeviceProperty&) = delete;

		protected:
			GraphicsDeviceWeakPtr _device;
			GraphicsDeviceProperties _deviceProperties;
		};
	}
}

#endif