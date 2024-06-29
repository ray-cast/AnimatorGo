#include "encode_module.h"

namespace unreal
{
	EncodeModule::EncodeModule() noexcept
	{
		this->reset();
	}

	EncodeModule::~EncodeModule() noexcept
	{
	}

	void
	EncodeModule::reset() noexcept
	{
		this->crf = 18;
		this->encodeMode = EncodeMode::H265;
		this->frame_type = 0;
		this->encode_speed = 0;
		this->quality = VideoQuality::High;
	}

	void 
	EncodeModule::load(nlohmann::json& reader) noexcept
	{
		if (reader["crf"].is_number_float())
			this->crf = reader["crf"].get<nlohmann::json::number_float_t>();
		if (reader["frame_type"].is_number_unsigned())
			this->frame_type = reader["frame_type"].get<nlohmann::json::number_unsigned_t>();
		if (reader["encode_speed"].is_number_unsigned())
			this->encode_speed = reader["encode_speed"].get<nlohmann::json::number_unsigned_t>();
		if (reader["encodeMode"].is_number_unsigned())
		{
			switch (reader["encodeMode"].get<nlohmann::json::number_unsigned_t>())
			{
			case EncodeMode::H264: this->encodeMode = EncodeMode::H264; break;
			case EncodeMode::H265: this->encodeMode = EncodeMode::H265; break;
			case EncodeMode::Frame: this->encodeMode = EncodeMode::Frame; break;
			default:
				break;
			}
		}
		if (reader["quality"].is_number_unsigned())
		{
			switch (reader["quality"].get<nlohmann::json::number_unsigned_t>())
			{
			case VideoQuality::Low: this->quality = VideoQuality::Low; break;
			case VideoQuality::Medium: this->quality = VideoQuality::Medium; break;
			case VideoQuality::High: this->quality = VideoQuality::High; break;
			default:
				break;
			}
		}
	}

	void 
	EncodeModule::save(nlohmann::json& writer) noexcept
	{
		writer["crf"] = this->crf.getValue();
		writer["frame_type"] = this->frame_type.getValue();
		writer["encode_speed"] = this->encode_speed.getValue();
		writer["quality"] = this->quality.getValue();
		writer["encodeMode"] = this->encodeMode.getValue();
	}

	void
	EncodeModule::disconnect() noexcept
	{
		this->enable.disconnect();
		this->crf.disconnect();
		this->encodeMode.disconnect();
		this->frame_type.disconnect();
		this->encode_speed.disconnect();
		this->quality.disconnect();
	}
}