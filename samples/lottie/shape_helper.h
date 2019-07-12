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

	bool hd;

	std::string ty;
	std::string nm;
	std::string mn;

	std::uint32_t np;
	std::uint32_t cix;
	std::uint32_t ix;

	struct PathHelper
	{
		std::uint32_t ix;
		bool hd;
	};

	struct RectangleHelper
	{
		std::uint32_t direction;
		octoon::animation::Keyframes<float2> size;
		octoon::animation::Keyframes<float2> position;
		octoon::animation::Keyframes<float1> roundness;
	};

	struct PolystarHelper
	{
		std::uint32_t type;
		std::uint32_t direction;
		octoon::animation::Keyframes<float1> points;
		octoon::animation::Keyframes<float2> position;
		octoon::animation::Keyframes<float1> rotation;
		octoon::animation::Keyframes<float1> innerRadius;
		octoon::animation::Keyframes<float1> outerRadius;
		octoon::animation::Keyframes<float1> innerRoundness;
		octoon::animation::Keyframes<float1> outerRoundness;
	};

	struct EllipseHelper
	{
		std::uint32_t direction;
		octoon::animation::Keyframes<float2> size;
		octoon::animation::Keyframes<float2> position;
	};

	struct MergeHelper
	{
		bool hd;
		std::uint32_t mode;
	};

	struct StrokeHelper
	{
		octoon::animation::Keyframes<float3> color;
		octoon::animation::Keyframes<float1> opacity;
		octoon::animation::Keyframes<float1> width;
		std::uint32_t lineCap;
		std::uint32_t lineJoin;
	};

	struct FillHelper
	{
		octoon::animation::Keyframes<float3> color;
		octoon::animation::Keyframes<float1> opacity;
	};

	struct GradientStrokeHelper
	{
		std::uint32_t type;
		octoon::animation::Keyframes<float1> opacity;
		octoon::animation::Keyframes<float1> width;
		octoon::animation::Keyframes<float2> start;
		octoon::animation::Keyframes<float2> end;
		octoon::animation::Keyframes<float1> miterLimit;
		std::uint32_t lineCap;
		std::uint32_t lineJoin;
	};

	struct GradientFillHelper
	{
		std::uint32_t type;
		std::uint32_t fillRule;
		octoon::animation::Keyframes<float1> opacity;
		octoon::animation::Keyframes<float2> start;
		octoon::animation::Keyframes<float2> end;
	};

	struct TwistHelper
	{
		octoon::animation::Keyframes<float1> angle;
		octoon::animation::Keyframes<float2> center;
	};

	struct TrimHelper
	{
		std::uint32_t trimMultipleShapes;
		octoon::animation::Keyframes<float1> start;
		octoon::animation::Keyframes<float1> end;
		octoon::animation::Keyframes<float1> offset;
	};

	struct RepeaterHelper
	{
		std::uint32_t composite;
		octoon::animation::Keyframes<float1> copies;
		octoon::animation::Keyframes<float1> offset;
	};

	struct RoundedCornersHelper
	{
		octoon::animation::Keyframes<float1> radius;
	};

	struct TransformHelper
	{
		octoon::animation::Keyframes<float2> pos;
		octoon::animation::Keyframes<float2> anchor;
		octoon::animation::Keyframes<float2> scale;
		octoon::animation::Keyframes<float1> skew;
		octoon::animation::Keyframes<float1> skewAxis;
		octoon::animation::Keyframes<float1> rz;
		octoon::animation::Keyframes<float1> opacity;
	};

	EllipseHelper ellipse;
	RectangleHelper rectangle;
	PolystarHelper star;
	std::vector<PathHelper> paths;
	FillHelper fill;
	GradientFillHelper gfill;
	GradientStrokeHelper gstroke;
	TwistHelper twist;
	TrimHelper trim;
	StrokeHelper stroke;
	RepeaterHelper repeater;
	RoundedCornersHelper roundedCorners;
	MergeHelper merge;
	TransformHelper transform;	

	ShapeHelper()
	{
	}

	ShapeHelper(const json& layer)
	{
		this->prepare(layer);
	}

	void prepare(const json& it)
	{
		ty = it["ty"].get<json::string_t>();

		if (ty == "gr")
		{
			nm = it["nm"].get<json::string_t>();
			mn = it["mn"].get<json::string_t>();
			ix = it["ix"].get<json::number_unsigned_t>();
			np = it["np"].get<json::number_unsigned_t>();
			cix = it["cix"].get<json::number_unsigned_t>();
			hd = it["hd"].get<json::boolean_t>();

			for (auto& group : it["it"])
				this->prepare(group);
		}
		else if (ty == "sh")
		{
			auto index = it["ind"].get<json::number_unsigned_t>();
			if (index <= paths.size())
				paths.resize(index + 1);

			PathHelper sh;
			sh.ix = it["ix"].get<json::number_unsigned_t>();
			sh.hd = it["hd"].get<json::boolean_t>();
			paths[index] = std::move(sh);
		}
		else if (ty == "rc")
		{
			rectangle.size = KeyframeHelper::preparefloat2(it["s"]);
			rectangle.position = KeyframeHelper::preparefloat2(it["p"]);
			rectangle.roundness = KeyframeHelper::preparefloat1(it["r"]);
			rectangle.direction = it["d"].get<json::number_unsigned_t>();
		}
		else if (ty == "el")
		{
			ellipse.size = KeyframeHelper::preparefloat2(it["s"]);
			ellipse.position = KeyframeHelper::preparefloat2(it["p"]);
			ellipse.direction = it["d"].get<json::number_unsigned_t>();
		}
		else if (ty == "sr")
		{
			star.type = it["sy"].get<json::number_unsigned_t>();
			star.direction = it["d"].get<json::number_unsigned_t>();
			star.points = KeyframeHelper::preparefloat1(it["pt"]);
			star.position = KeyframeHelper::preparefloat2(it["p"]);
			star.rotation = KeyframeHelper::preparefloat1(it["r"]);
			star.innerRadius = KeyframeHelper::preparefloat1(it["ir"]);
			star.outerRadius = KeyframeHelper::preparefloat1(it["or"]);
			star.innerRoundness = KeyframeHelper::preparefloat1(it["is"]);
			star.outerRoundness = KeyframeHelper::preparefloat1(it["os"]);
		}
		else if (ty == "fl")
		{
			fill.color = KeyframeHelper::preparefloat3(it["c"]);
			fill.opacity = KeyframeHelper::preparefloat1(it["o"], 1.0f / 100.0f);
		}
		else if (ty == "gf")
		{
			gfill.type = it["t"].get<json::number_unsigned_t>();
			gfill.fillRule = it["r"].get<json::number_unsigned_t>();
			gfill.opacity = KeyframeHelper::preparefloat1(it["o"], 1.0f / 100.0f);
			gfill.start = KeyframeHelper::preparefloat2(it["s"]);
			gfill.end = KeyframeHelper::preparefloat2(it["e"]);
		}
		else if (ty == "gs")
		{
			gstroke.type = it["t"].get<json::number_unsigned_t>();
			gstroke.opacity = KeyframeHelper::preparefloat1(it["o"], 1.0f / 100.0f);
			gstroke.width = KeyframeHelper::preparefloat1(it["w"]);
			gstroke.lineCap = it["lc"].get<json::number_unsigned_t>();
			gstroke.lineJoin = it["lj"].get<json::number_unsigned_t>();
			gstroke.start = KeyframeHelper::preparefloat2(it["s"]);
			gstroke.end = KeyframeHelper::preparefloat2(it["e"]);
			gstroke.miterLimit = KeyframeHelper::preparefloat1(it["ml"]);
		}
		else if (ty == "st")
		{
			stroke.color = KeyframeHelper::preparefloat3(it["c"]);
			stroke.opacity = KeyframeHelper::preparefloat1(it["o"], 1.0f / 100.0f);
			stroke.width = KeyframeHelper::preparefloat1(it["w"]);
			stroke.lineCap = it["lc"].get<json::number_unsigned_t>();
			stroke.lineJoin = it["lj"].get<json::number_unsigned_t>();
		}
		else if (ty == "mm")
		{
			merge.hd = it["hd"].get<json::boolean_t>();
			merge.mode = it["mm"].get<json::number_unsigned_t>();
		}
		else if (ty == "tm")
		{
			trim.start = KeyframeHelper::preparefloat1(it["s"], 1.0f / 100.0f);
			trim.end = KeyframeHelper::preparefloat1(it["e"], 1.0f / 100.0f);
			trim.offset = KeyframeHelper::preparefloat1(it["o"], octoon::math::radians(1.0f));
			trim.trimMultipleShapes = it["m"].get<json::number_unsigned_t>();
		}
		else if (ty == "tw")
		{
			twist.angle = KeyframeHelper::preparefloat1(it["a"], octoon::math::radians(1.0f));
			twist.center = KeyframeHelper::preparefloat2(it["c"]);
		}
		else if (ty == "rp")
		{
			repeater.copies = KeyframeHelper::preparefloat1(it["c"]);
			repeater.offset = KeyframeHelper::preparefloat1(it["o"]);
			repeater.composite = it["m"].get<json::number_unsigned_t>();
		}
		else if (ty == "rd")
		{
			roundedCorners.radius = KeyframeHelper::preparefloat1(it["r"]);
		}
		else if (ty == "tr")
		{
			transform.pos = KeyframeHelper::preparefloat2(it["p"]);
			transform.anchor = KeyframeHelper::preparefloat2(it["a"]);
			transform.scale = KeyframeHelper::preparefloat2(it["a"]);
			transform.skew = KeyframeHelper::preparefloat1(it["sk"]);
			transform.skewAxis = KeyframeHelper::preparefloat1(it["sa"]);
			transform.rz = KeyframeHelper::preparefloat1(it["r"]);
			transform.opacity = KeyframeHelper::preparefloat1(it["o"]);
		}
	}
};

#endif