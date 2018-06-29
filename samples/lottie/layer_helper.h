#ifndef LAYER_HELPER_H_
#define LAYER_HELPER_H_

#include "keyframe_helper.h"
#include "transform_helper.h"
#include "shape_helper.h"
#include "solid_helper.h"
#include "still_helper.h"
#include "text_helper.h"
#include "camera_helper.h"

#include <octoon/game_object.h>

class LayerHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	enum class LayerTypes
	{
		precomp = 0,
		solid = 1,
		still = 2,
		nullLayer = 3,
		shape = 4,
		text = 5,
		audio = 6,
		pholderVideo = 7,
		imageSeq = 8,
		video = 9,
		pholderStill = 10,
		guide = 11,
		adjustment = 12,
		camera = 13,
		light = 14
	};

	LayerTypes type;
	bool threeLayer;

	TransformHelper transform;
	SoildHelper solid;
	StillHelper still;
	TextHelper text;
	CameraHelper camera;

	LayerHelper()
	{
	}

	LayerHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		octoon::GameObjectPtr object;

		type = (LayerTypes)layer["ty"].get<json::number_unsigned_t>();
		threeLayer = layer["ddd"].get<json::number_unsigned_t>();
		
		transform.prepare(layer);

		switch (type)
		{
		case LayerTypes::solid:
		{
			solid.prepare(layer);
		}
		break;
		case LayerTypes::still:
		{
			still.prepare(layer);
		}
		break;
		case LayerTypes::text:
		{
			text.prepare(layer);
		}
		break;
		case LayerTypes::shape:
		{
			ShapeHelper shape;
		}
		break;
		case LayerTypes::camera:
		{
			camera.prepare(layer);
		}
		break;
		default:
			break;
		}
	}
};

#endif