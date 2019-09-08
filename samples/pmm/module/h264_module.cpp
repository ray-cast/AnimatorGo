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
		this->fps = 30;
		this->kbps = 0;

		this->qp_min = 10;
		this->qp_max = 10;

		this->frame_type = 0;
		this->encode_speed = 0;
	}

	void 
	H264Module::load(octoon::runtime::json& reader) noexcept
	{
		this->fps = reader["fps"];
		this->kbps = reader["kbps"];
		this->qp_min = reader["qp_min"];
		this->qp_max = reader["qp_max"];
		this->frame_type = reader["frame_type"];
		this->encode_speed = reader["encode_speed"];
	}

	void 
	H264Module::save(octoon::runtime::json& writer) noexcept
	{
		writer["fps"] = this->fps;
		writer["kbps"] = this->kbps;
		writer["qp_min"] = this->qp_min;
		writer["qp_max"] = this->qp_max;
		writer["frame_type"] = this->frame_type;
		writer["encode_speed"] = this->encode_speed;
	}
}