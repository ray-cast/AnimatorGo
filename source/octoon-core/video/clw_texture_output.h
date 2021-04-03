#ifndef OCTOON_VIDEO_CLW_TEXTURE_OUTPUT_H_
#define OCTOON_VIDEO_CLW_TEXTURE_OUTPUT_H_

#include <CLW.h>
#include "clw_output.h"

namespace octoon
{
	class ClwTextureOutput : public ClwOutput
	{
	public:
		ClwTextureOutput(CLWContext context, std::uint32_t texture, std::uint32_t w, std::uint32_t h);

		void syncData(CLWKernel& copyKernel);

	private:
		CLWImage2D image_;
	};
}

#endif