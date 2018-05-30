#include <octoon/model/path_edge.h>

namespace octoon
{
	namespace model
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

		namespace deform
		{
			void postprocess(PathEdge& it, const std::function<math::float2(const math::float2&)>& func) noexcept
			{
				switch (it.type)
				{
				case octoon::model::PathEdge::Point:
				{
					auto v = func(it.point.pt.xy());
					it.point.pt.x = v.x;
					it.point.pt.y = v.y;
				}
				break;
				case octoon::model::PathEdge::Line:
				{
					auto v1 = func(it.line.pt1.xy());
					auto v2 = func(it.line.pt2.xy());

					it.line.pt1.x = v1.x;
					it.line.pt1.y = v1.y;

					it.line.pt2.x = v2.x;
					it.line.pt2.y = v2.y;
				}
				break;
				case octoon::model::PathEdge::Quadratic:
				{
					auto v1 = func(it.quad.pt1.xy());
					auto v2 = func(it.quad.pt2.xy());
					auto c1 = func(it.quad.control.xy());

					it.quad.pt1.x = v1.x;
					it.quad.pt1.y = v1.y;

					it.quad.pt2.x = v2.x;
					it.quad.pt2.y = v2.y;

					it.quad.control.x = c1.x;
					it.quad.control.y = c1.y;
				}
				break;
				case octoon::model::PathEdge::Cubic:
				{
					auto v1 = func(it.cubic.pt1.xy());
					auto v2 = func(it.cubic.pt2.xy());
					auto c1 = func(it.cubic.control1.xy());
					auto c2 = func(it.cubic.control2.xy());

					it.cubic.pt1.x = v1.x;
					it.cubic.pt1.y = v1.y;

					it.cubic.pt2.x = v2.x;
					it.cubic.pt2.y = v2.y;

					it.cubic.control1.x = c1.x;
					it.cubic.control1.y = c1.y;

					it.cubic.control2.x = c2.x;
					it.cubic.control2.y = c2.y;
				}
				break;
				default:
					break;
				}
			}

			std::function<void(PathEdge&)> twist(float x, float y, bool rotate) noexcept
			{
				auto twist = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float xx = math::lerp(pt.x, pt.x - pt.x * pt.y, y);
						float yy = math::lerp(pt.y, pt.y + pt.x * pt.y, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(twist, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fan(float x, float ratio, bool rotate) noexcept
			{
				auto fan_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = math::snorm2unorm(pt.y) * pt.x;
						float weight = math::cos(pt.x * math::PI * 0.5f) * ratio;
						float xx = math::lerp(pt.x, pt.x + unorm, math::abs(x));
						float yy = math::lerp(pt.y, pt.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto fan_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = math::snorm2unorm(pt.x) * pt.y;
						float weight = math::cos(pt.y * math::PI * 0.5f) * ratio;
						float xx = math::lerp(pt.x, pt.x - weight, x);
						float yy = math::lerp(pt.y, pt.y + unorm * ratio, math::abs(x));
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(fan_x, std::placeholders::_1);
				else
					return std::bind(fan_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> coveLow(float x, float ratio, bool rotate) noexcept
			{
				auto coveLow_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
						float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - abs(pt.x)), math::abs(x));
						float yy = math::lerp(pt.y, pt.y - weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto coveLow_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(pt.x));
						float weight = math::cos(pt.y * math::PI * 0.5f) * unorm / ratio;
						float xx = math::lerp(pt.x, pt.x + weight, x);
						float yy = math::lerp(pt.y, pt.y + pt.y * unorm * (1.0f - abs(pt.y)) , math::abs(x));
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(coveLow_x, std::placeholders::_1);
				else
					return std::bind(coveLow_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> coveHigh(float x, float ratio, bool rotate) noexcept
			{
				auto coveHigh_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(pt.y));
						float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - abs(pt.x)), math::abs(x));
						float yy = math::lerp(pt.y, pt.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto coveHigh_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.x));
						float weight = math::cos(pt.y * math::PI * 0.5f) * unorm / ratio;
						float xx = math::lerp(pt.x, pt.x - weight, x);
						float yy = math::lerp(pt.y, pt.y + pt.y * unorm * (1.0f - abs(pt.y)), math::abs(x));
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(coveHigh_x, std::placeholders::_1);
				else
					return std::bind(coveHigh_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> cove(float x, float ratio, bool rotate) noexcept
			{
				auto cove_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::cos(pt.x * math::PI * 0.5f) * ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * (1.0f - math::abs(pt.x)), abs(x));
						float yy = math::lerp(pt.y, pt.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto cove_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::cos(pt.y * math::PI * 0.5f) / ratio;
						float xx = math::lerp(pt.x, pt.x - weight, math::abs(x));
						float yy = math::lerp(pt.y, pt.y + pt.y * (1.0f - math::abs(pt.y)), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(cove_x, std::placeholders::_1);
				else
					return std::bind(cove_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulege(float x, float ratio, bool rotate) noexcept
			{
				auto bulege_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::cos(pt.x * math::PI * 0.5f) * ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * (1.0f - math::abs(pt.x)), x);
						float yy = math::lerp(pt.y, pt.y + pt.y * weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));					
				};

				auto bulege_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::cos(pt.y * math::PI * 0.5f) / ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * weight, x);
						float yy = math::lerp(pt.y, pt.y + pt.y * (1.0f - math::abs(pt.y)), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(bulege_x, std::placeholders::_1);
				else
					return std::bind(bulege_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulegeLow(float x, float ratio, bool rotate) noexcept
			{
				auto bulegeLow_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt)
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
						float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * ratio;
						float xx = math::lerp(pt.x, pt.x + pt.x * unorm * 0.5f, x);
						float yy = math::lerp(pt.y, pt.y - weight * unorm, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto bulegeLow_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt)
					{
						float unorm = math::saturate(math::snorm2unorm(pt.x));
						float weight = math::cos(pt.y * math::PI * 0.5f) * unorm;
						float xx = math::lerp(pt.x, pt.x + weight * unorm * 0.5f, x);
						float yy = math::lerp(pt.y, pt.y + pt.y * unorm * ratio, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(bulegeLow_x, std::placeholders::_1);
				else
					return std::bind(bulegeLow_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulegeHigh(float x, float ratio, bool rotate) noexcept
			{
				auto bulegeHigh_x = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(pt.y));
						float weight = math::cos(pt.x * math::PI * 0.5f) * unorm;
						float xx = math::lerp(pt.x, pt.x + unorm * pt.x  * 0.5f, x);
						float yy = math::lerp(pt.y, pt.y + unorm * weight * ratio, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				auto bulegeHigh_y = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.x));
						float weight = math::cos(pt.y * math::PI * 0.5f) * unorm;
						float xx = math::lerp(pt.x, pt.x - unorm * weight * 0.5f, x);
						float yy = math::lerp(pt.y, pt.y + unorm * pt.y * ratio, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				if (!rotate)
					return std::bind(bulegeHigh_x, std::placeholders::_1);
				else
					return std::bind(bulegeHigh_y, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> flag(float x, float y) noexcept
			{
				auto flag = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float xx = math::lerp(pt.x, pt.x + math::sin(pt.y * math::PI), y);
						float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(flag, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> wave(float x, float y) noexcept
			{
				auto wave = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float xx = math::lerp(pt.x, pt.x + math::sin(pt.y * math::PI) * (1.0f - abs(pt.x)), y);
						float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI) * (1.0f - abs(pt.y)), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(wave, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fish(float x, float y) noexcept
			{
				auto fish = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float xx = math::lerp(pt.x, pt.x + pt.x * math::sin(pt.y * math::PI), y);
						float yy = math::lerp(pt.y, pt.y + pt.y * math::sin(pt.x * -math::PI), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(fish, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> slope(float x, float y) noexcept
			{
				auto slope = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float xx = math::lerp(pt.x, pt.x + math::sin(pt.y * math::PI * 0.5f) - math::sin(math::PI * 0.5f), y * 2.0f);
						float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI * 0.5f) - math::sin(math::PI * 0.5f), x * 2.0f);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(slope, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fishEye(float x) noexcept
			{
				auto fishEye = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::rsqrt(1.0f + math::dot(pt, pt));
						float solidAngle = weight * weight * weight;

						float xx = math::lerp(pt.x, pt.x + pt.x * solidAngle * (1.0f - math::pow(pt.x, 4)), x);
						float yy = math::lerp(pt.y, pt.y + pt.y * solidAngle * (1.0f - math::pow(pt.y, 4)), x);

						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(fishEye, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> expandOut(float x, float ratio, bool rotate) noexcept
			{
				auto expandOut = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::length(math::float2::One) - math::pow(math::length(pt), math::abs(pt.x));
						float xx = math::lerp(pt.x, pt.x + pt.x * weight, x);
						float yy = math::lerp(pt.y, pt.y + pt.y * weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(expandOut, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> expandIn(float x, float ratio, bool rotate) noexcept
			{
				auto expandIn = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						float weight = math::length(math::float2::One) - math::pow(math::length(pt), math::abs(pt.x));
						float xx = math::lerp(pt.x, pt.x - pt.x * weight, x * (rotate ? 1.0f : -1.0f));
						float yy = math::lerp(pt.y, pt.y + pt.y * weight, x * (rotate ? 1.0f : -1.0f));
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(expandIn, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> spin(float x, float ratio, bool rotate) noexcept
			{
				auto spin = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						math::float2 sc;
						math::sinCos(&sc.x, &sc.y, x * math::PI * math::saturate(1.1f - math::length(pt)) * (rotate ? -0.5f : 0.5f));

						float xx = pt.x * sc.y - pt.y * sc.x;
						float yy = pt.x * sc.x + pt.y * sc.y;

						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(spin, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> panini(float x, float y) noexcept
			{
				auto panini = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& pt) -> math::float2
					{
						// Thanks to : http://tksharpless.net/vedutismo/Pannini/panini.pdf
						float invLen = math::rsqrt(1.0f + pt.x * pt.x);
						float SinPhi = pt.x * invLen;
						float TanTheta = pt.y * invLen;
						float CosPhi = math::sqrt(1.0f - SinPhi * SinPhi);
						float S = (x + 1.0f) / (x + CosPhi);

						// return S * math::float2(math::lerp(SinPhi, SinPhi / CosPhi, s), TanTheta);
						return S * math::float2(SinPhi, math::lerp(TanTheta, TanTheta / CosPhi, y * 10.0f));
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(panini, std::placeholders::_1);
			}
		}
	}
}