#ifndef TRANSFORM_HELPER_H_
#define TRANSFORM_HELPER_H_

#include "keyframe_helper.h"

class TransformHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	octoon::animation::Keyframes<float3> pos;
	octoon::animation::Keyframes<float3> anchor;
	octoon::animation::Keyframes<float3> scale;
	octoon::animation::Keyframes<float3> orientation;
	octoon::animation::Keyframes<float1> rx;
	octoon::animation::Keyframes<float1> ry;
	octoon::animation::Keyframes<float1> rz;
	octoon::animation::Keyframes<float1> opacity;

	TransformHelper()
	{
	}

	TransformHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		auto& ks = layer["ks"];
		if (ks.is_null()) return;
		if (ks.find("a") != ks.end()) anchor = KeyframeHelper::preparefloat3(ks["a"]);
		if (ks.find("p") != ks.end()) pos = KeyframeHelper::preparefloat3(ks["p"]);
		if (ks.find("s") != ks.end()) scale = KeyframeHelper::preparefloat3(ks["s"], 1.0f / 100.0f);
		if (ks.find("or") != ks.end()) orientation = KeyframeHelper::preparefloat3(ks["or"], octoon::math::radians(1.0f));
		if (ks.find("r") != ks.end()) rz = KeyframeHelper::preparefloat1(ks["r"], octoon::math::radians(1.0f));
		if (ks.find("rx") != ks.end()) rx = KeyframeHelper::preparefloat1(ks["rx"], octoon::math::radians(1.0f));
		if (ks.find("ry") != ks.end()) ry = KeyframeHelper::preparefloat1(ks["ry"], octoon::math::radians(1.0f));
		if (ks.find("rz") != ks.end()) rz = KeyframeHelper::preparefloat1(ks["rz"], octoon::math::radians(1.0f));
		if (ks.find("o") != ks.end()) opacity = KeyframeHelper::preparefloat1(ks["o"], 1.0f / 100.0f);

		if (!orientation.empty())
		{
			auto cur = orientation.front();
			for (auto& it : orientation)
			{
				if ((cur.value.x - it.value.x) > octoon::math::PI)
					it.value.x += octoon::math::PI_2;
				if ((cur.value.y - it.value.y) > octoon::math::PI)
					it.value.y += octoon::math::PI_2;
				if ((cur.value.z - it.value.z) > octoon::math::PI)
					it.value.z += octoon::math::PI_2;
				if ((cur.value.x - it.value.x) < -octoon::math::PI)
					it.value.x -= octoon::math::PI_2;
				if ((cur.value.y - it.value.y) < -octoon::math::PI)
					it.value.y -= octoon::math::PI_2;
				if ((cur.value.z - it.value.z) < -octoon::math::PI)
					it.value.z -= octoon::math::PI_2;
				cur = it;
			}
		}
	}
};

#endif