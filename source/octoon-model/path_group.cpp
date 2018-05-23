#include <octoon/model/path_group.h>

namespace octoon
{
	namespace model
	{
		PathGroup::PathGroup() noexcept
		{
		}

		PathGroup::PathGroup(Paths&& contour) noexcept
		{
			this->setPath(std::move(contour));
		}

		PathGroup::PathGroup(PathPtr&& contour) noexcept
		{
			this->setPath(std::move(contour));
		}

		PathGroup::PathGroup(const Paths& contour) noexcept
		{
			this->setPath(contour);
		}

		PathGroup::~PathGroup() noexcept
		{
		}

		void
		PathGroup::setPath(Paths&& paths) noexcept
		{
			paths_ = std::move(paths);
		}

		void
		PathGroup::setPath(PathPtr&& paths) noexcept
		{
			paths_.push_back(std::move(paths));
		}

		void
		PathGroup::setPath(const Paths& paths) noexcept
		{
			paths_.clear();

			for (auto& it : paths)
				paths_.push_back(std::make_unique<Path>(*it));
		}

		void
		PathGroup::addPath(Paths&& paths) noexcept
		{
			for (auto& it : paths)
				paths_.push_back(std::move(it));
		}

		void
		PathGroup::addPath(PathPtr&& path) noexcept
		{
			paths_.push_back(std::move(path));
		}

		void
		PathGroup::addPath(const Paths& paths) noexcept
		{
			for (auto& it : paths)
				paths_.push_back(std::make_unique<Path>(*it));
		}

		Paths&
		PathGroup::getPaths() noexcept
		{
			return paths_;
		}

		const Paths&
		PathGroup::getPaths() const noexcept
		{
			return paths_;
		}

		Path&
		PathGroup::at(std::size_t index)  noexcept
		{
			assert(index < paths_.size());
			return *paths_[index];
		}

		const Path&
		PathGroup::at(std::size_t index) const noexcept
		{
			assert(index < paths_.size());
			return *paths_[index];
		}

		PathGroup&
		PathGroup::invoke(const std::function<void(PathGroup&)>& func) noexcept
		{
			func(*this);
			return *this;
		}

		PathGroup&
		PathGroup::invoke(const std::function<void(PathEdge&)>& func) noexcept
		{
			for (auto& path : paths_)
				path->invoke(func);

			return *this;
		}

		math::AABB aabb(const PathGroup& group) noexcept
		{
			math::AABB aabb;

			for (auto& contour : group.getPaths())
			{
				for (auto& edge : contour->edges())
				{
					switch (edge.type)
					{
					case PathEdge::Point:
						aabb.encapsulate(edge.point.pt);
						break;
					case PathEdge::Quadratic:
						aabb.encapsulate(edge.quad.pt1);
						aabb.encapsulate(edge.quad.pt2);
						break;
					case PathEdge::Cubic:
						aabb.encapsulate(edge.cubic.pt1);
						aabb.encapsulate(edge.cubic.pt2);
						break;
					default:
						break;
					}
				}
			}

			return aabb;
		}

		math::AABB aabb(const PathGroups& groups) noexcept
		{
			math::AABB aabb;

			for (auto& group : groups)
			{
				for (auto& contour : group->getPaths())
				{
					for (auto& edge : contour->edges())
					{
						switch (edge.type)
						{
						case PathEdge::Point:
							aabb.encapsulate(edge.point.pt);
							break;
						case PathEdge::Quadratic:
							aabb.encapsulate(edge.quad.pt1);
							aabb.encapsulate(edge.quad.pt2);
							break;
						case PathEdge::Cubic:
							aabb.encapsulate(edge.cubic.pt1);
							aabb.encapsulate(edge.cubic.pt2);
							break;
						default:
							break;
						}
					}
				}
			}

			return aabb;
		}

		namespace transform
		{
			std::function<void(PathGroups&)> begin(std::uint32_t steps) noexcept
			{
				auto begin = [](PathGroups& groups, std::uint32_t steps) noexcept
				{
					for (auto& group : groups)
					{
						for (auto& path : group->getPaths())
						{
							PathEdges edges;

							auto it = path->edges().begin();
							auto end = path->edges().end();

							for (; it != end; ++it)
							{
								if ((*it).type == PathEdge::Point)
								{
									auto pt1 = *(it);
									
									if (it + 1 != end)
									{
										auto begin = pt1.point.pt;
										auto end = (*(it + 1)).point.pt;
										auto step = (end - begin) / (steps + 1);

										for (std::size_t n = 0; n < steps; n++, begin += step + step)
										{
											edges.emplace_back(begin, begin + step, begin + step + step);
										}										
									}
									else
									{
										edges.push_back(pt1);
									}
								}
							}

							path->setEdge(std::move(edges));
						}
					}
				};

				return std::bind(begin, std::placeholders::_1, steps);
			}
		}
	}
}