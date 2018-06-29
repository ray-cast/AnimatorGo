#ifndef TEXT_HELPER_H_
#define TEXT_HELPER_H_

#include "keyframe_helper.h"

class TextHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	std::string text;
	std::string font;
	std::uint32_t size;
	octoon::math::float3 color;

	TextHelper()
	{
	}

	TextHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		auto& t = layer["t"]["d"]["k"][0]["s"];
		auto& fc = t["fc"];
		text = t["t"].get<json::string_t>();
		font = t["f"].get<json::string_t>();
		size = t["s"].get<json::number_unsigned_t>();
		color = octoon::math::float3(fc[0].get<json::number_float_t>(), fc[1].get<json::number_float_t>(), fc[2].get<json::number_float_t>());
	}
};

#endif