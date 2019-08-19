#include "h264_module.h"

namespace MysticLit
{
	H264Module::H264Module() noexcept
	{
	}

	H264Module::~H264Module() noexcept
	{
	}

	void
	H264Module::reset() noexcept
	{
		this->qp_min = 21;
		this->qp_max = 21;

		this->frame_type = 0;
		this->encode_speed = 0;
	}
}