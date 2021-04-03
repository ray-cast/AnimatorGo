#include <octoon/model/contour.h>
#include <octoon/model/path.h>

namespace octoon
{
	Contour::Contour() noexcept
	{
	}

	Contour::Contour(const Path& path, std::uint16_t bezierSteps) noexcept
	{
		this->addPoints(path, bezierSteps);
	}

	Contour::Contour(math::float3s&& points) noexcept
	{
		points_ = std::move(points);
	}

	Contour::Contour(std::initializer_list<math::float3>&& list) noexcept
	{
		points_ = std::move(list);
	}

	Contour::Contour(const math::float3& points) noexcept
	{
		points_.push_back(points);
	}

	Contour::Contour(const math::float3s& points) noexcept
	{
		points_ = points;
	}

	Contour::Contour(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept
	{
		this->addPoints(pt1, pt2, steps);
	}

	Contour::Contour(const math::float3& a, const math::float3& b, const math::float3& c, std::uint16_t bezierSteps) noexcept
	{
		this->addPoints(a, b, c, bezierSteps);
	}

	Contour::Contour(const math::float3& a, const math::float3& b, const math::float3& c, const math::float3& d, std::uint16_t bezierSteps) noexcept
	{
		this->addPoints(a, b, c, bezierSteps);
	}

	Contour::Contour(const std::initializer_list<math::float3>& list) noexcept
	{
		for (auto& it : list)
			this->addPoints(it);
	}

	math::float3&
	Contour::at(std::size_t index) noexcept
	{
		return points_[index];
	}

	const math::float3&
	Contour::at(std::size_t index) const noexcept
	{
		return points_[index];
	}

	math::float3s&
	Contour::points() noexcept
	{
		return points_;
	}

	const math::float3s&
	Contour::points() const noexcept
	{
		return points_;
	}

	Contour&
	Contour::invoke(const std::function<math::float2(const math::float2&)>& func) noexcept
	{
		for (auto& it : points_)
		{
			auto v = func(it.xy());
			it.x = v.x;
			it.y = v.y;
		}

		return *this;
	}

	Contour&
	Contour::invoke(const std::function<math::float3(const math::float3&)>& func) noexcept
	{
		for (auto& it : points_)
			it = func(it);

		return *this;
	}

	void 
	Contour::addPoints(const Path& path, std::uint16_t bezierSteps) noexcept
	{
		for (auto& edge : path.edges())
		{
			switch (edge.type)
			{
			case PathEdge::Type::Point:
				this->addPoints(edge.point.pt);
				break;
			case PathEdge::Type::Line:
				this->addPoints(edge.line.pt1, edge.line.pt2, bezierSteps);
				break;
			case PathEdge::Type::Quadratic:
				this->addPoints(edge.quad.pt1, edge.quad.control, edge.quad.pt2, bezierSteps);
				break;
			case PathEdge::Type::Cubic:
				this->addPoints(edge.cubic.pt1, edge.cubic.control1, edge.cubic.control2, edge.quad.pt2, bezierSteps);
				break;
			default:
				assert(false);
			}
		}
	}

	void
	Contour::addPoints(const math::float3& point) noexcept
	{
		points_.push_back(point);
	}

	void
	Contour::addPoints(const math::float3s& points) noexcept
	{
		for (auto& point : points)
			points_.push_back(point);
	}

	void
	Contour::addPoints(const math::float3& pt1, const math::float3& pt2, std::uint16_t steps) noexcept
	{
		for (std::uint16_t i = 0; i <= steps; i++)
		{
			float t = (float)i / steps;
			this->addPoints(math::lerp(pt1, pt2, t));
		}
	}

	void
	Contour::addPoints(const math::float3& A, const math::float3& B, const math::float3& C, std::uint16_t bezierSteps) noexcept
	{
		for (std::uint16_t i = 0; i <= bezierSteps; i++)
		{
			float t = (float)i / bezierSteps;
			float t2 = 1.0f - t;

			math::float3 u = t2 * A + t * B;
			math::float3 v = t2 * B + t * C;

			this->addPoints(t2 * u + t * v);
		}
	}

	void
	Contour::addPoints(const math::float3& A, const math::float3& B, const math::float3& C, const math::float3& D, std::uint16_t bezierSteps) noexcept
	{
		for (std::uint16_t i = 0; i <= bezierSteps; i++)
		{
			float t = (float)i / bezierSteps;
			float t2 = 1.0f - t;

			math::float3 u = t2 * A + t * B;
			math::float3 v = t2 * B + t * C;
			math::float3 w = t2 * C + t * D;

			math::float3 m = t2 * u + t * v;
			math::float3 n = t2 * v + t * w;

			this->addPoints(t2 * m + t * n);
		}
	}

	ContourPtr
	Contour::clone() const noexcept
	{
		auto contour = std::make_shared<Contour>();
		contour->points_ = this->points_;
		return contour;
	}
}