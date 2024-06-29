#ifndef OCTOON_CLW_TEXTURE_OUTPUT_H_
#define OCTOON_CLW_TEXTURE_OUTPUT_H_

#include <CLW.h>
#include "clw_output.h"

namespace octoon
{
	class ClwTextureOutput : public ClwOutput
	{
	public:
		ClwTextureOutput(CLWContext context, std::uint32_t texture, std::uint32_t w, std::uint32_t h);
		~ClwTextureOutput();

		void syncData(CLWKernel& copyKernel);

	private:
		CLWImage2D image_;
		std::vector<cl_mem> objects_;
	};
}

#endif