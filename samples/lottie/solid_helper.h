#ifndef SOLID_HELPER_H_
#define SOLID_HELPER_H_

#include "keyframe_helper.h"

class SoildHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	std::uint32_t sw;
	std::uint32_t sh;
	std::string sc;

	SoildHelper()
	{
	}

	SoildHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		sw = layer["sw"].get <json::number_unsigned_t>();
		sh = layer["sh"].get <json::number_unsigned_t>();
		sc = layer["sc"].get <json::string_t>();
	}
};

#endif