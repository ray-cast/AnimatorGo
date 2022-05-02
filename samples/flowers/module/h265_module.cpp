#include "h265_module.h"

namespace flower
{
	H265Module::H265Module() noexcept
	{
		this->reset();
	}

	H265Module::~H265Module() noexcept
	{
	}

	void
	H265Module::setVideoQuality(VideoQuality quality_)
	{
		switch (quality_)
		{
		case flower::Low:
			this->quality = quality_;
			break;
		case flower::Medium:
			this->quality = quality_;
			break;
		case flower::High:
			this->quality = quality_;
			break;
		default:
			break;
		}
	}

	void
	H265Module::reset() noexcept
	{
		this->enable = false;

		this->crf = 15;
		this->frame_type = 0;
		this->encode_speed = 0;
		this->quality = VideoQuality::Medium;
	}

	void 
	H265Module::load(octoon::runtime::json& reader) noexcept
	{
		if (reader.find("crf") != reader.end())
			this->crf = reader["crf"];
		if (reader.find("frame_type") != reader.end())
			this->frame_type = reader["frame_type"];
		if (reader.find("encode_speed") != reader.end())
			this->encode_speed = reader["encode_speed"];
	}

	void 
	H265Module::save(octoon::runtime::json& writer) noexcept
	{
		writer["crf"] = this->crf;
		writer["frame_type"] = this->frame_type;
		writer["encode_speed"] = this->encode_speed;
	}
}