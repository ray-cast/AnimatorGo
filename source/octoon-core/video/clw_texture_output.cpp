#include "clw_texture_output.h"

#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#include <OpenGL/OpenGL.h>
#elif WIN32
#include <Windows.h>
#include "GL/glew.h"
#else
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#endif

namespace octoon
{
	ClwTextureOutput::ClwTextureOutput(CLWContext context, std::uint32_t texture, std::uint32_t w, std::uint32_t h)
		: ClwOutput(context, w, h)
		, image_(context.CreateImage2DFromGLTexture(texture))
	{
	}

	void
	ClwTextureOutput::syncData(CLWKernel& copyKernel)
	{
		std::vector<cl_mem> objects;
		objects.push_back(image_);
		context_.AcquireGLObjects(0, objects);

		int argc = 0;
		copyKernel.SetArg(argc++, this->data());
		copyKernel.SetArg(argc++, static_cast<cl_int>(this->width()));
		copyKernel.SetArg(argc++, static_cast<cl_int>(this->height()));
		copyKernel.SetArg(argc++, 2.2f);
		copyKernel.SetArg(argc++, image_);

		std::size_t globalsize = this->width() * this->height();
		context_.Launch1D(0, ((globalsize + 63) / 64) * 64, 64, copyKernel);

		context_.ReleaseGLObjects(0, objects);
		context_.Finish(0);
	}
}