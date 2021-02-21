#include <octoon/model/path_edge.h>

namespace octoon::model
{
	PathEdge::PathEdge() noexcept
	{
	}

	PathEdge::PathEdge(const math::float3& pt1) noexcept
	{
		this->makeEdge(pt1);
	}

	PathEdge::PathEdge(const math::float3& pt1, const math::float3& pt2) noexcept
	{
		this->makeEdge(pt1, pt2);
	}

	PathEdge::PathEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept
	{
		this->makeEdge(pt1, control, pt2);
	}

	PathEdge::PathEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept
	{
		this->makeEdge(pt1, control1, control2, pt2);
	}

	void
	PathEdge::makeEdge(const math::float3& pt) noexcept
	{
		this->type = Type::Point;
		this->point.pt = pt;
	}

	void
	PathEdge::makeEdge(const math::float3& pt1, const math::float3& pt2) noexcept
	{
		this->type = Type::Line;
		this->line.pt1 = pt1;
		this->line.pt2 = pt2;
	}

	void
	PathEdge::makeEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept
	{
		this->type = Type::Quadratic;
		this->quad.pt1 = pt1;
		this->quad.pt2 = pt2;
		this->quad.control = control;
	}

	void
	PathEdge::makeEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept
	{
		this->type = Type::Cubic;
		this->cubic.pt1 = pt1;
		this->cubic.pt2 = pt2;
		this->cubic.control1 = control1;
		this->cubic.control2 = control2;
	}

	PathEdge&
	PathEdge::invoke(const std::function<math::float2(const math::float2&)>& func) noexcept
	{
		switch (type)
		{
		case octoon::model::PathEdge::Point:
		{
			auto v = func(point.pt.xy());
			point.pt.x = v.x;
			point.pt.y = v.y;
		}
		break;
		case octoon::model::PathEdge::Line:
		{
			auto v1 = func(line.pt1.xy());
			auto v2 = func(line.pt2.xy());

			line.pt1.x = v1.x;
			line.pt1.y = v1.y;

			line.pt2.x = v2.x;
			line.pt2.y = v2.y;
		}
		break;
		case octoon::model::PathEdge::Quadratic:
		{
			auto v1 = func(quad.pt1.xy());
			auto v2 = func(quad.pt2.xy());
			auto c1 = func(quad.control.xy());

			quad.pt1.x = v1.x;
			quad.pt1.y = v1.y;

			quad.pt2.x = v2.x;
			quad.pt2.y = v2.y;

			quad.control.x = c1.x;
			quad.control.y = c1.y;
		}
		break;
		case octoon::model::PathEdge::Cubic:
		{
			auto v1 = func(cubic.pt1.xy());
			auto v2 = func(cubic.pt2.xy());
			auto c1 = func(cubic.control1.xy());
			auto c2 = func(cubic.control2.xy());

			cubic.pt1.x = v1.x;
			cubic.pt1.y = v1.y;

			cubic.pt2.x = v2.x;
			cubic.pt2.y = v2.y;

			cubic.control1.x = c1.x;
			cubic.control1.y = c1.y;

			cubic.control2.x = c2.x;
			cubic.control2.y = c2.y;
		}
		break;
		default:
			break;
		}

		return *this;
	}

	PathEdge&
	PathEdge::invoke(const std::function<math::float3(const math::float3&)>& func) noexcept
	{
		switch (type)
		{
		case octoon::model::PathEdge::Point:
		{
			point.pt = func(point.pt);
		}
		break;
		case octoon::model::PathEdge::Line:
		{
			line.pt1 = func(line.pt1);
			line.pt2 = func(line.pt2);
		}
		break;
		case octoon::model::PathEdge::Quadratic:
		{
			quad.pt1 = func(quad.pt1);
			quad.pt2 = func(quad.pt2);
			quad.control = func(quad.control);
		}
		break;
		case octoon::model::PathEdge::Cubic:
		{
			cubic.pt1 = func(cubic.pt1);
			cubic.pt2 = func(cubic.pt2);
			cubic.control1 = func(cubic.control1);
			cubic.control2 = func(cubic.control2);
		}
		break;
		default:
			break;
		}

		return *this;
	}

	namespace deform
	{
		std::function<math::float3(const math::float3&)> twist(float x, float y, bool rotate) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float xx = math::lerp(pt.x, pt.x - pt.x * pt.y, y);
				float yy = math::lerp(pt.y, pt.y + pt.x * pt.y, x);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> fan(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				math::float2 n = math::float2::Zero;
				math::sinCos(&n.x, &n.y, math::PI * pt.x * 0.5f);

				float len = x < 0.0f ? math::snorm2unorm(pt.y) : 1.0f - math::snorm2unorm(pt.y);
				float xx = math::lerp(pt.x, n.x * (len * math::lerp(1.0f, 0.5f, math::abs(pt.x)) + 1.0f), math::abs(x));
				float yy = math::lerp(pt.y, n.y * (len + ratio + 1.0f - math::abs(pt.x)) * (x < 0.0f ? 1.0f : -1.0f), math::abs(x));

				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				math::float2 n = math::float2::Zero;
				math::sinCos(&n.x, &n.y, math::PI * pt.y * 0.5f);

				float len = x < 0.0f ? math::snorm2unorm(pt.x) : 1.0f - math::snorm2unorm(pt.x);
				float xx = math::lerp(pt.x, n.y * (len + 1.0f) * math::snorm2unorm(pt.x), math::abs(x));
				float yy = math::lerp(pt.y, n.x * (len + ratio) * (x < 0.0f ? 1.0f : -1.0f), math::abs(x));

				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> coveLow(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				float unorm = math::saturate(math::snorm2unorm(pt.y));
				float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * (ratio + math::abs(pt.x) * math::abs(x));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.x)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - math::abs(pt.x)) * atten, math::abs(x));
				float yy = math::lerp(pt.y, pt.y + weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				float unorm = math::saturate(math::snorm2unorm(pt.x));
				float weight = math::cos(pt.y * math::PI * 0.5f) * unorm / ratio;
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.y)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x + weight, x);
				float yy = math::lerp(pt.y, pt.y + pt.y * unorm * (1.0f - math::abs(pt.y)) * atten, math::abs(x));
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> coveHigh(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
				float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * (ratio + math::abs(pt.x) * math::abs(x));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.x)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - math::abs(pt.x)) * atten, math::abs(x));
				float yy = math::lerp(pt.y, pt.y - weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.x));
				float weight = math::cos(pt.y * math::PI * 0.5f) * unorm / ratio;
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.y)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x - weight, x);
				float yy = math::lerp(pt.y, pt.y + pt.y * unorm * (1.0f - math::abs(pt.y)) * atten, math::abs(x));
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> cove(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::cos(pt.x * math::PI * 0.5f) * (ratio - (1.0f - math::abs(pt.x)) * (1.0f - math::abs(x)));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.x));
				float xx = math::lerp(pt.x, pt.x + pt.x * (1.0f - math::abs(pt.x)) * atten, math::abs(x));
				float yy = math::lerp(pt.y, pt.y - weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::cos(pt.y * math::PI * 0.5f) / (ratio + (1.0f - math::abs(pt.y)) * (1.0f - math::abs(x)));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.y));
				float xx = math::lerp(pt.x, pt.x - weight, x);
				float yy = math::lerp(pt.y, pt.y + pt.y * (1.0f - math::abs(pt.y)) * atten, math::abs(x));
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> bulege(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::cos(pt.x * math::PI * 0.5f) * (ratio + math::abs(pt.x) * math::abs(x));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.x)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x + pt.x * (1.0f - math::abs(pt.x)) * atten, math::abs(x));
				float yy = math::lerp(pt.y, pt.y + pt.y * weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::cos(pt.y * math::PI * 0.5f) / (ratio + math::abs(pt.x) * math::abs(x));
				float atten = math::lerp(0.5f, 1.0f, math::abs(pt.x)) * math::abs(x);
				float xx = math::lerp(pt.x, pt.x + pt.x * weight, x);
				float yy = math::lerp(pt.y, pt.y + pt.y * (1.0f - math::abs(pt.y)) * atten, math::abs(x));
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> bulegeLow(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt)
			{
				float unorm = math::saturate(math::snorm2unorm(pt.y));
				float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * unorm * ratio;
				float atten = x < 0.0f ? (1.0f - math::abs(pt.x)) : 1.0f;
				float xx = math::lerp(pt.x, pt.x + unorm * pt.x * atten * 0.5f, math::abs(x));
				float yy = math::lerp(pt.y, pt.y + unorm * weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt)
			{
				float unorm = math::saturate(math::snorm2unorm(pt.x));
				float weight = (1.0f - math::cos(pt.y * math::PI * 0.5f)) * unorm * unorm;
				float xx = math::lerp(pt.x, pt.x - unorm * weight, x);
				float yy = math::lerp(pt.y, pt.y + unorm * unorm * pt.y * ratio * ratio * 0.5f, math::abs(x));
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> bulegeHigh(float x, float ratio, bool rotate) noexcept
		{
			auto method_x = [=](const math::float3& pt) -> math::float3
			{
				float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
				float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * unorm * ratio;
				float atten = x < 0.0f ? (1.0f - math::abs(pt.x)) : 1.0f;
				float xx = math::lerp(pt.x, pt.x + unorm * pt.x * atten * 0.5f, math::abs(x));
				float yy = math::lerp(pt.y, pt.y - unorm * weight, x);
				return math::float3(xx, yy, pt.z);
			};

			auto method_y = [=](const math::float3& pt) -> math::float3
			{
				float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.x));
				float weight = (1.0f - math::cos(pt.y * math::PI * 0.5f)) * unorm * unorm;
				float xx = math::lerp(pt.x, pt.x + unorm * weight, x);
				float yy = math::lerp(pt.y, pt.y + unorm * unorm * pt.y * ratio * ratio * 0.5f, x);
				return math::float3(xx, yy, pt.z);
			};

			if (!rotate)
				return std::bind(method_x, std::placeholders::_1);
			else
				return std::bind(method_y, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> flag(float x, float y) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float xx = math::lerp(pt.x, pt.x - math::sin(pt.y * math::PI), y);
				float yy = math::lerp(pt.y, pt.y - math::sin(pt.x * math::PI), x);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> wave(float x, float y) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float xx = math::lerp(pt.x, pt.x - math::sin(pt.y * math::PI) * (1.0f - math::abs(pt.x)), y);
				float yy = math::lerp(pt.y, pt.y - math::sin(pt.x * math::PI) * (1.0f - math::abs(pt.y)), x * 0.5f);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> fish(float x, float y, float weight) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float xx = math::lerp(pt.x, pt.x + pt.x * math::sin(pt.y * -math::PI), y * weight);
				float yy = math::lerp(pt.y, pt.y + pt.y * math::sin(pt.x * -math::PI), x * weight);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> slope(float x, float y) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float xx = math::lerp(pt.x, pt.x + math::sin(pt.y * math::PI *-0.5f) - math::sin(math::PI * 0.5f), y * 2.0f);
				float yy = math::lerp(pt.y, pt.y - math::sin(pt.x * math::PI * 0.5f) + math::sin(math::PI * 0.5f), x * 2.0f);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> fishEye(float x) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::rsqrt(1.0f + math::dot(pt.xy(), pt.xy()));
				float solidAngle = weight * weight * weight;

				float xx = math::lerp(pt.x, pt.x + pt.x * solidAngle * (1.0f - math::pow(pt.x, 4)), x);
				float yy = math::lerp(pt.y, pt.y + pt.y * solidAngle * (1.0f - math::pow(pt.y, 4)), x);

				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> expandOut(float x, float ratio, bool rotate) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::length(math::float2::One) - math::length(pt.xy());
				float xx = math::lerp(pt.x, pt.x + pt.x * weight * math::lerp(0.5f, 1.0f, math::abs(pt.x)), x);
				float yy = math::lerp(pt.y, pt.y + pt.y * weight * math::lerp(0.5f, 1.0f, math::abs(pt.y)), x);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> expandIn(float x, float ratio, bool rotate) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				float weight = math::length(math::float2::One) - math::pow(math::length(pt.xy()), math::abs(pt.x));
				float xx = math::lerp(pt.x, pt.x - pt.x * weight * math::lerp(0.5f, 1.0f, math::abs(pt.x)), rotate ? -x : x);
				float yy = math::lerp(pt.y, pt.y + pt.y * weight * math::lerp(0.5f, 1.0f, math::abs(pt.y)), rotate ? -x : x);
				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> spin(float x, float ratio, bool rotate) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				math::float2 sc;
				math::sinCos(&sc.x, &sc.y, x * math::PI * math::saturate(1.1f - math::length(pt.xy())) * (rotate ? -0.5f : 0.5f));

				float xx = pt.x * sc.y - pt.y * sc.x;
				float yy = pt.x * sc.x + pt.y * sc.y;

				return math::float3(xx, yy, pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}

		std::function<math::float3(const math::float3&)> panini(float x, float y) noexcept
		{
			auto method = [=](const math::float3& pt) -> math::float3
			{
				// Thanks to : http://tksharpless.net/vedutismo/Pannini/panini.pdf
				float invLen = math::rsqrt(1.0f + pt.x * pt.x);
				float SinPhi = pt.x * invLen;
				float TanTheta = pt.y * invLen;
				float CosPhi = math::sqrt(1.0f - SinPhi * SinPhi);
				float S = (x + 1.0f) / (x + CosPhi);

				// return S * math::float2(math::lerp(SinPhi, SinPhi / CosPhi, s), TanTheta);
				return math::float3(S * math::float2(SinPhi, math::lerp(TanTheta, TanTheta / CosPhi, y * 10.0f)), pt.z);
			};

			return std::bind(method, std::placeholders::_1);
		}
	}
}