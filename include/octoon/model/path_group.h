#ifndef OCTOON_PATH_GROUP_H_
#define OCTOON_PATH_GROUP_H_

#include <octoon/model/path.h>

namespace octoon
{
	class OCTOON_EXPORT PathGroup final
	{
	public:
		PathGroup() noexcept;
		explicit PathGroup(Paths&& path) noexcept;
		explicit PathGroup(PathPtr&& path) noexcept;
		explicit PathGroup(const Paths& path) noexcept;
		virtual ~PathGroup() noexcept;

		void setPath(Paths&& path) noexcept;
		void setPath(PathPtr&& path) noexcept;
		void setPath(const Paths& path) noexcept;

		void addPath(Paths&& path) noexcept;
		void addPath(PathPtr&& path) noexcept;
		void addPath(const Paths& path) noexcept;

		Path& at(std::size_t index) noexcept;
		const Path& at(std::size_t index) const noexcept;

		Paths& getPaths() noexcept;
		const Paths& getPaths() const noexcept;

		PathGroup& invoke(const std::function<void(PathEdge&)>& func) noexcept;
		PathGroup& invoke(const std::function<void(PathGroup&)>& func) noexcept;
		PathGroup& invoke(const std::function<math::float2(const math::float2&)>& func) noexcept;
		PathGroup& invoke(const std::function<math::float3(const math::float3&)>& func) noexcept;

	public:
		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroup& operator+=(PathGroup& path, T scale) noexcept { for (auto& it : path.paths_) (*it) += scale; return path; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroup& operator-=(PathGroup& path, T scale) noexcept { for (auto& it : path.paths_) (*it) -= scale; return path; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroup& operator*=(PathGroup& path, T scale) noexcept { for (auto& it : path.paths_) (*it) *= scale; return path; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroup& operator/=(PathGroup& path, T scale) noexcept { for (auto& it : path.paths_) (*it) /= scale; return path; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroups& operator+=(PathGroups& groups, T scale) noexcept { for (auto& it : groups) *it += scale; return groups; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroups& operator-=(PathGroups& groups, T scale) noexcept { for (auto& it : groups) *it -= scale; return groups; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroups& operator*=(PathGroups& groups, T scale) noexcept { for (auto& it : groups) *it *= scale; return groups; }

		template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
		friend PathGroups& operator/=(PathGroups& groups, T scale) noexcept { for (auto& it : groups) *it /= scale; return groups; }

		friend PathGroup& operator<<(PathGroup& group, const std::function<void(PathEdge&)>& func) noexcept
		{
			return group.invoke(func); 
		}

		friend PathGroup& operator<<(PathGroup& group, const std::function<void(PathGroup&)>& func) noexcept
		{
			return group.invoke(func); 
		}

		friend PathGroups& operator<<(PathGroups& groups, const std::function<void(PathEdge&)>& func) noexcept
		{
			for (auto& it : groups)
				it->invoke(func);

			return groups;
		}

		friend PathGroups& operator<<(PathGroups& groups, const std::function<void(PathGroup&)>& func) noexcept
		{
			for (auto& it : groups)
				it->invoke(func);

			return groups;
		}

		friend PathGroups& operator<<(PathGroups& groups, const std::function<math::float2(const math::float2&)>& func) noexcept
		{
			for (auto& it : groups)
				it->invoke(func);

			return groups;
		}

		friend PathGroups& operator<<(PathGroups& groups, const std::function<math::float3(const math::float3&)>& func) noexcept
		{
			for (auto& it : groups)
				it->invoke(func);

			return groups;
		}

		friend PathGroups& operator<<(PathGroups& groups, const std::function<void(PathGroups&)>& func) noexcept
		{
			func(groups);
			return groups;
		}

		friend std::ostream& operator << (std::ostream& os, const PathGroup& group) noexcept
		{
			os << group.getPaths();
			return os;
		}

		friend std::ostream& operator << (std::ostream& os, const PathGroups& groups) noexcept
		{
			for (auto& it : groups)
				os << it;
			return os;
		}

	private:
		PathGroup(const PathGroup&) = delete;
		PathGroup& operator=(const PathGroup&) = delete;

	private:
		Paths paths_;
	};

	OCTOON_EXPORT math::AABB aabb(const PathGroup& group) noexcept;
	OCTOON_EXPORT math::AABB aabb(const PathGroups& groups) noexcept;

	namespace deform
	{
		OCTOON_EXPORT std::function<void(PathGroups&)> smoother(std::uint32_t steps) noexcept;
	}
}

#endif