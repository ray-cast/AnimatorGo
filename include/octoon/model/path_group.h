#ifndef OCTOON_MODEL_PATH_GROUP_H_
#define OCTOON_MODEL_PATH_GROUP_H_

#include <octoon/model/path.h>
#include <functional>

namespace octoon
{
	namespace model
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

			friend PathGroups& operator<<(PathGroups& groups, const std::function<void(PathGroups&)>& func) noexcept
			{
				func(groups);
				return groups;
			}

		private:
			PathGroup(const PathGroup&) = delete;
			PathGroup& operator=(const PathGroup&) = delete;

		private:
			Paths paths_;
		};

		OCTOON_EXPORT math::AABB aabb(const PathGroup& group) noexcept;
		OCTOON_EXPORT math::AABB aabb(const PathGroups& groups) noexcept;
	}
}

#endif