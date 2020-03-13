#include <octoon/model/path.h>
#include <octoon/math/math.h>

namespace octoon
{
	namespace model
	{
		Path::Path() noexcept
		{
		}

		Path::Path(PathEdges&& edges) noexcept
			: edges_(std::move(edges))
		{
		}

		Path::Path(const PathEdge& edges) noexcept
		{
			edges_.push_back(edges);
		}

		Path::Path(const PathEdges& edges) noexcept
			: edges_(edges)
		{
		}

		Path::Path(const math::float3& point) noexcept
		{
			this->addEdge(point);
		}

		Path::Path(const math::float3s& points) noexcept
		{
			this->addEdge(points);
		}

		Path::Path(const math::float3& pt1, const math::float3& pt2) noexcept
		{
			this->addEdge(pt1, pt2);
		}

		Path::Path(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept
		{
			this->addEdge(pt1, control1, pt2);
		}

		Path::Path(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept
		{
			this->addEdge(pt1, control1, control2, pt2);
		}

		Path::~Path() noexcept
		{
		}

		PathEdge&
		Path::at(std::size_t index) noexcept
		{
			return edges_[index];
		}

		const PathEdge&
		Path::at(std::size_t index) const noexcept
		{
			return edges_[index];
		}

		PathEdges&
		Path::edges() noexcept
		{
			return edges_;
		}

		const PathEdges&
		Path::edges() const noexcept
		{
			return edges_;
		}

		void
		Path::setEdge(PathEdges&& edges) noexcept
		{
			edges_ = std::move(edges);
		}

		void
		Path::setEdge(const PathEdge& edges) noexcept
		{
			edges_.clear();
			edges_.push_back(std::move(edges));
		}

		void
		Path::setEdge(const PathEdges& edges) noexcept
		{
			edges_ = std::move(edges);
		}

		void
		Path::setEdge(const math::float3& pt1) noexcept
		{
			edges_.clear();
			this->addEdge(pt1);
		}

		void
		Path::setEdge(const math::float3s& points) noexcept
		{
			edges_.clear();
			this->addEdge(points);
		}

		void
		Path::setEdge(const math::float3& pt1, const math::float3& pt2) noexcept
		{
			edges_.clear();
			this->addEdge(pt1, pt2);
		}

		void
		Path::setEdge(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept
		{
			edges_.clear();
			this->addEdge(pt1, control1, pt2);
		}

		void
		Path::setEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept
		{
			edges_.clear();
			this->addEdge(pt1, control1, control2, pt2);
		}

		void
		Path::addEdge(PathEdges&& edges) noexcept
		{
			for (auto& edge : edges)
				edges_.push_back(std::move(edge));
		}

		void
		Path::addEdge(const PathEdge& edge) noexcept
		{
			edges_.push_back(edge);
		}

		void
		Path::addEdge(const PathEdges& edges) noexcept
		{
			for (auto& edge : edges)
				edges_.push_back(edge);
		}

		void
		Path::addEdge(const math::float3& pt1) noexcept
		{
			edges_.emplace_back(pt1);
		}

		void
		Path::addEdge(const math::float3s& points) noexcept
		{
			for (auto& it : points)
				edges_.emplace_back(it);
		}

		void
		Path::addEdge(const math::float3& pt1, const math::float3& pt2) noexcept
		{
			edges_.emplace_back(pt1, pt2);
		}

		void
		Path::addEdge(const math::float3& pt1, const math::float3& control, const math::float3& pt2) noexcept
		{
			edges_.emplace_back(pt1, control, pt2);
		}

		void
		Path::addEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept
		{
			edges_.emplace_back(pt1, control1, control2, pt2);
		}

		Path& 
		Path::invoke(const std::function<void(Path&)>& func) noexcept
		{
			func(*this);
			return *this;
		}

		Path& 
		Path::invoke(const std::function<void(PathEdge&)>& func) noexcept
		{
			for (auto& it : edges_)
				func(it);
			return *this;
		}

		Path&
		Path::invoke(const std::function<math::float2(const math::float2&)>& func) noexcept
		{
			for (auto& it : edges_)
				it.invoke(func);
			return *this;
		}

		Path&
		Path::invoke(const std::function<math::float3(const math::float3&)>& func) noexcept
		{
			for (auto& it : edges_)
				it.invoke(func);
			return *this;
		}

		PathPtr
		Path::clone() const noexcept
		{
			auto path = std::make_shared<Path>();
			path->edges_ = this->edges_;
			return path;
		}
	}
}