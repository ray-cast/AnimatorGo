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

	octoon::model::Keyframes<float3> pos;
	octoon::model::Keyframes<float3> anchor;
	octoon::model::Keyframes<float3> scale;
	octoon::model::Keyframes<float3> orientation;
	octoon::model::Keyframes<float1> rx;
	octoon::model::Keyframes<float1> ry;
	octoon::model::Keyframes<float1> rz;
	octoon::model::Keyframes<float1> opacity;

	TransformHelper()
	{
	}

	TransformHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& j)
	{
		if (j.find("a") != j.end()) anchor = KeyframeHelper::preparefloat3(j["a"]);
		if (j.find("p") != j.end()) pos = KeyframeHelper::preparefloat3(j["p"]);
		if (j.find("s") != j.end()) scale = KeyframeHelper::preparefloat3(j["s"], 1.0f / 100.0f);
		if (j.find("or") != j.end()) orientation = KeyframeHelper::preparefloat3(j["or"], octoon::math::radians(1.0f));
		if (j.find("r") != j.end()) rz = KeyframeHelper::preparefloat1(j["r"], octoon::math::radians(1.0f));
		if (j.find("rx") != j.end()) rx = KeyframeHelper::preparefloat1(j["rx"], octoon::math::radians(1.0f));
		if (j.find("ry") != j.end()) ry = KeyframeHelper::preparefloat1(j["ry"], octoon::math::radians(1.0f));
		if (j.find("rz") != j.end()) rz = KeyframeHelper::preparefloat1(j["rz"], octoon::math::radians(1.0f));
		if (j.find("o") != j.end()) opacity = KeyframeHelper::preparefloat1(j["o"], 1.0f / 100.0f);
	}
};

#endif