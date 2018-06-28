#ifndef SHAPE_HELPER_H_
#define SHAPE_HELPER_H_

#include "keyframe_helper.h"

class ShapeHelper
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

	ShapeHelper()
	{
	}

	ShapeHelper(const json& layer)
	{
		this->prepare(layer);
	}

	void prepare(const json& layer)
	{
		auto& shape = layer["shape"];
		if (shape.is_null()) return;

		auto ty = shape["ty"].get<json::string_t>();
		if (ty == "sh")// shape
		{
		}
		else if (ty == "rc") // rect
		{
		}
		else if (ty == "el") // ellipse
		{
		}
		else if (ty == "sr") // star
		{
		}
		else if (ty == "fl") // fill
		{
		}
		else if (ty == "gf") // gfill
		{
		}
		else if (ty == "gs") // gStroke
		{
		}
		else if (ty == "st") // stroke
		{
		}
		else if (ty == "mm") // merge
		{
		}
		else if (ty == "tm") // trim
		{
		}
		else if (ty == "tw") // twist
		{
		}
		else if (ty == "gr") // group
		{
		}
		else if (ty == "rp") // repeater
		{
		}
		else if (ty == "rd") // roundedCorners
		{
		}
		else
		{
		}
	}
};

#endif