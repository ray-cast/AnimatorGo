#ifndef STILL_HELPER_H_
#define STILL_HELPER_H_

#include "keyframe_helper.h"

class StillHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	std::string refid;

	StillHelper()
	{
	}

	StillHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		refid = layer["refId"].get<json::string_t>();
	}
};

#endif