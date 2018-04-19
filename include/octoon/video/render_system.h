#ifndef OCTOON_RENDER_SYSTEM_H_
#define OCTOON_RENDER_SYSTEM_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>

#if defined(__LINUX__)
#	include <X11/Xlib.h>
#	include <GL/glew.h>
#	include <GL/glxew.h>
#endif

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderSystem final : public runtime::Singleton<RenderSystem>
		{
		public:
			RenderSystem() noexcept;
			~RenderSystem() noexcept;

			void setup(std::uint32_t w, std::uint32_t h) noexcept;
			void close() noexcept;

			void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
			void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void render() noexcept;

#if defined(__WINDOWS__)
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
#elif defined(__LINUX__)
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
#elif defined(__APPLE__)
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
			void setupGLEnvironment(CreateParam& param) noexcept(false);
			void closeGLEnvironment(const CreateParam& param) noexcept;

		private:
			CreateParam glcontext_;
			std::uint32_t width_, height_;

			math::float3s vertices_;
			math::float3s colors_;

			std::uint32_t fbo_;
			std::uint32_t colorTexture_;

			std::uint32_t fboMSAA_;
			std::uint32_t colorTextureMSAA_;
			std::uint32_t depthTextureMSAA_;
		};
	}
}

#endif