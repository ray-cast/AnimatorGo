#ifndef CAMERA_HELPER_H_
#define CAMERA_HELPER_H_

#include "keyframe_helper.h"

class CameraHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	enum CameraTypes
	{
		CameraZoom = 0,
		CameraDepthOfField = 1,
		CameraFocusDistance = 2,
		CameraAperture = 3,
		CameraBlurLevel = 4,
		IrisShape = 5,
		IrisRotation = 6,
		IrisRoundness = 7,
		IrisAspectRatio = 8,
		IrisDiffractionFringe = 9,
		IrisHighlightGain = 10,
		IrisHighlightThreshold = 11,
		IrisHightlightSaturation = 12
	};

	octoon::model::Keyframes<float1> zoom;
	octoon::model::Keyframes<float1> depthOfField;
	octoon::model::Keyframes<float1> focusDistance;
	octoon::model::Keyframes<float1> aperture;
	octoon::model::Keyframes<float1> blurLevel;
	octoon::model::Keyframes<float1> irisShape;
	octoon::model::Keyframes<float1> irisRotation;
	octoon::model::Keyframes<float1> irisRoundness;
	octoon::model::Keyframes<float1> irisAspectRatio;
	octoon::model::Keyframes<float1> irisDiffractionFringe;
	octoon::model::Keyframes<float1> irisHighlightGain;
	octoon::model::Keyframes<float1> irisHighlightThreshold;
	octoon::model::Keyframes<float1> irisHightlightSaturation;

	CameraHelper()
	{
	}

	CameraHelper(const json& j)
	{
		this->prepare(j);
	}

	void prepare(const json& layer)
	{
		auto& co = layer["co"];
		for (auto& it : co)
		{
			auto ty = it["ty"].get<json::number_unsigned_t>();
			switch (ty)
			{
			case CameraZoom:
			{
				zoom = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case CameraDepthOfField:
			{
				depthOfField = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case CameraFocusDistance:
			{
				focusDistance = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case CameraAperture:
			{
				aperture = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case CameraBlurLevel:
			{
				blurLevel = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisShape:
			{
				irisShape = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisRotation:
			{
				irisRotation = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisRoundness:
			{
				irisRoundness = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisAspectRatio:
			{
				irisAspectRatio = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisDiffractionFringe:
			{
				irisDiffractionFringe = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisHighlightGain:
			{
				irisHighlightGain = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisHighlightThreshold:
			{
				irisHighlightThreshold = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			case IrisHightlightSaturation:
			{
				irisHightlightSaturation = KeyframeHelper::preparefloat1(it["v"]);
			}
			break;
			default:
				break;
			}
		}
	}
};

#endif