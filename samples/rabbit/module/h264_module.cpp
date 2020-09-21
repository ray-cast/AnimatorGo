#include "h264_module.h"

namespace rabbit
{
	H264Module::H264Module() noexcept
	{
		this->reset();
	}

	H264Module::~H264Module() noexcept
	{
	}

	void
	H264Module::setVideoQuality(VideoQuality quality_)
	{
		switch (quality_)
		{
		case rabbit::Low:
			this->quality = quality_;
			break;
		case rabbit::Medium:
			this->quality = quality_;
			break;
		case rabbit::High:
			this->quality = quality_;
			break;
		default:
			break;
		}
	}

	void
	H264Module::reset() noexcept
	{
		this->enable = false;

		this->crf = 15;
		this->frame_type = 0;
		this->encode_speed = 0;
		this->quality = VideoQuality::Medium;
	}

	void 
	H264Module::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("crf") != reader.end())
			this->crf = reader["crf"];
		if (reader.find("frame_type") != reader.end())
			this->frame_type = reader["frame_type"];
		if (reader.find("encode_speed") != reader.end())
			this->encode_speed = reader["encode_speed"];
	}

	void 
	H264Module::save(octoon::runtime::json& writer) noexcept
	{
		writer["crf"] = this->crf;
		writer["frame_type"] = this->frame_type;
		writer["encode_speed"] = this->encode_speed;
	}
}