#include "clw_texture_output.h"

namespace octoon
{
	ClwTextureOutput::ClwTextureOutput(CLWContext context, std::uint32_t texture, std::uint32_t w, std::uint32_t h)
		: ClwOutput(context, w, h)
		, image_(context.CreateImage2DFromGLTexture(texture))
	{
		objects_.push_back(image_);
	}

	ClwTextureOutput::~ClwTextureOutput()
	{
		objects_.clear();
	}

	void
	ClwTextureOutput::syncData(CLWKernel& copyKernel)
	{
		context_.AcquireGLObjects(0, objects_);

		int argc = 0;
		copyKernel.SetArg(argc++, this->data());
		copyKernel.SetArg(argc++, static_cast<cl_int>(this->width()));
		copyKernel.SetArg(argc++, static_cast<cl_int>(this->height()));
		copyKernel.SetArg(argc++, 2.2f);
		copyKernel.SetArg(argc++, image_);

		std::size_t globalsize = this->width() * this->height();

		context_.Launch1D(0, ((globalsize + 63) / 64) * 64, 64, copyKernel);
		context_.ReleaseGLObjects(0, objects_);
	}
}