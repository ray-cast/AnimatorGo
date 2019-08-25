#include "h264_module.h"

namespace MysticLit
{
	H264Module::H264Module() noexcept
	{
		this->reset();
	}

	H264Module::~H264Module() noexcept
	{
	}

	void
	H264Module::reset() noexcept
	{
		this->fps = 24.0f;
		this->kbps = 0;

		this->qp_min = 10;
		this->qp_max = 10;

		this->frame_type = 0;
		this->encode_speed = 0;
	}
}