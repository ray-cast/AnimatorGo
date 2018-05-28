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

		namespace transform
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

			std::function<void(PathEdge&)> twist(float x, float y) noexcept
			{
				auto twist = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float xx = math::lerp(point.x, point.x - point.x * point.y, y);
						float yy = math::lerp(point.y, point.y + point.x * point.y, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(twist, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fan(float x, float y, float ratio) noexcept
			{
				auto fan = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(point.y));
						float weight = math::cos(point.x * math::PI * 0.5f) * ratio;
						float xx = math::lerp(point.x, point.x + point.x * unorm * (1.3f - abs(point.x)), abs(x));
						float yy = math::lerp(point.y, point.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(fan, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> lowCove(float x, float y, float ratio) noexcept
			{
				auto lowCove = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(point.y));
						float weight = math::cos(point.x * math::PI * 0.5f) * unorm * ratio;
						float xx = math::lerp(point.x, point.x + point.x * unorm * (1.0f - abs(point.x)) * 0.3f, abs(x));
						float yy = math::lerp(point.y, point.y - weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(lowCove, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> highCove(float x, float y, float ratio) noexcept
			{
				auto highCove = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(point.y));
						float weight = math::cos(point.x * math::PI * 0.5f) * unorm * ratio;
						float xx = math::lerp(point.x, point.x + point.x * unorm * (1.0f - abs(point.x)) * 0.3f, abs(x));
						float yy = math::lerp(point.y, point.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(highCove, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> cove(float x, float y) noexcept
			{
				auto cove = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float weight = math::cos(point.x * math::PI * 0.5f) * 8.0f;
						float xx = math::lerp(point.x, point.x + point.x * (1.0f - math::abs(point.x)) * 0.5f, abs(x));
						float yy = math::lerp(point.y, point.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(cove, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulege(float x, float y) noexcept
			{
				auto bulege = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float weight = math::cos(point.x * math::PI * 0.5f) * math::length(point) * 8.0f;
						float xx = math::lerp(point.x, point.x + point.x * weight, y);
						float yy = math::lerp(point.y, point.y + point.y * weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(bulege, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulegeHigh(float x, float y) noexcept
			{
				auto bulegeHigh = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float unorm = math::saturate(math::snorm2unorm(point.y));
						float weight = math::cos(point.x * math::PI * 0.5f) * unorm * unorm * 6.8f;
						float xx = math::lerp(point.x, point.x + point.x * unorm * 0.3f, abs(x));
						float yy = math::lerp(point.y, point.y + weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(bulegeHigh, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> bulegeLow(float x, float y) noexcept
			{
				auto bulegeLow = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point)
					{
						float unorm = 1.0f - math::saturate(math::snorm2unorm(point.y));
						float weight = math::cos(point.x * math::PI * 0.5f) * unorm * unorm * 6.8f;
						float xx = math::lerp(point.x, point.x + point.x * unorm * 0.3f, abs(x));
						float yy = math::lerp(point.y, point.y - weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(bulegeLow, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fish(float x, float y) noexcept
			{
				auto fish = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float xx = math::lerp(point.x, point.x + point.x * math::sin(point.y * math::PI), y);
						float yy = math::lerp(point.y, point.y + point.y * math::sin(point.x * -math::PI), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(fish, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> flag(float x, float y) noexcept
			{
				auto flag = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float xx = math::lerp(point.x, point.x + math::sin(point.y * math::PI), y);
						float yy = math::lerp(point.y, point.y + math::sin(point.x * math::PI), x);
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
					auto method = [=](const math::float2& point) -> math::float2
					{
						float xx = math::lerp(point.x, point.x + math::sin(point.y * math::PI) * (1.0f - abs(point.x)), y);
						float yy = math::lerp(point.y, point.y + math::sin(point.x * math::PI) * (1.0f - abs(point.y)), x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(wave, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> slope(float x, float y) noexcept
			{
				auto slope = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float xx = math::lerp(point.x, point.x + math::sin(point.y * math::PI * 0.5f) - math::sin(math::PI * 0.5f), y * 2.0f);
						float yy = math::lerp(point.y, point.y + math::sin(point.x * math::PI * 0.5f) - math::sin(math::PI * 0.5f), x * 2.0f);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(slope, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> fishEye(float x, float y, float ratio) noexcept
			{
				auto fishEye = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float weight = 1.0f - math::length(point);
						float xx = math::lerp(point.x, point.x + point.x * weight, x);
						float yy = math::lerp(point.y, point.y + point.y * weight / ratio, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(fishEye, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> expandOut(float x, float y) noexcept
			{
				auto expandOut = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float weight = 1.5f - math::length(point);
						float xx = math::lerp(point.x, point.x + point.x * weight, x);
						float yy = math::lerp(point.y, point.y + point.y * weight, x);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(expandOut, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> expandIn(float x, float y) noexcept
			{
				auto expandIn = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						float weight = 1.5f - math::length(point);
						float xx = math::lerp(point.x, point.x + point.x * weight, x * 0.5f);
						float yy = math::lerp(point.y, point.y - point.y * weight, x * 0.5f);
						return math::float2(xx, yy);
					};

					postprocess(it, std::bind(method, std::placeholders::_1));
				};

				return std::bind(expandIn, std::placeholders::_1);
			}

			std::function<void(PathEdge&)> spin(float x, float y, float ratio) noexcept
			{
				auto spin = [=](PathEdge& it) noexcept
				{
					auto method = [=](const math::float2& point) -> math::float2
					{
						math::float2 spin;
						math::sinCos(&spin.x, &spin.y, x * (1.0f - math::length(point)) * 0.5f);

						float xx = math::lerp(point.x, point.x + point.y * spin.x, x);
						float yy = math::lerp(point.y, point.x + point.y * spin.y, x / ratio);

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
					auto method = [=](const math::float2& point) -> math::float2
					{
						// Thanks to : http://tksharpless.net/vedutismo/Pannini/panini.pdf
						float invLen = math::rsqrt(1.0f + point.x * point.x);
						float SinPhi = point.x * invLen;
						float TanTheta = point.y * invLen;
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