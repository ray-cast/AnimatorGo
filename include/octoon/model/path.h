#ifndef OCTOON_MODEL_PATH_H_
#define OCTOON_MODEL_PATH_H_

#include <octoon/model/path_edge.h>
#include <functional>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Path final
		{
		public:
			Path() noexcept;
			Path(const math::float3& pt1, const math::float3& pt2) noexcept; // Line Curve
			Path(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept; // Quadratic Curve
			Path(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve
			explicit Path(PathEdges&& edges) noexcept;
			explicit Path(const math::float3& pt1) noexcept;
			explicit Path(const math::float3s& pt1) noexcept;
			explicit Path(const PathEdge& edge) noexcept;
			explicit Path(const PathEdges& edges) noexcept;
			~Path() noexcept;

			PathEdge& at(std::size_t index) noexcept;
			const PathEdge& at(std::size_t index) const noexcept;

			PathEdges& edges() noexcept;
			const PathEdges& edges() const noexcept;

			void addEdge(PathEdges&& edges) noexcept;
			void addEdge(const PathEdge& edge) noexcept;
			void addEdge(const PathEdges& edge) noexcept;
			void addEdge(const math::float3& pt1) noexcept; // point
			void addEdge(const math::float3s& pt1) noexcept; // points
			void addEdge(const math::float3& pt1, const math::float3& pt2) noexcept; // Line Curve
			void addEdge(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept; // Quadratic Curve
			void addEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

			Path& invoke(const std::function<void(Path&)>& func) noexcept;
			Path& invoke(const std::function<void(PathEdge&)>& func) noexcept;

			Path& operator << (const std::function<void(Path&)>& func) noexcept { return this->invoke(func); }
			Path& operator << (const std::function<void(PathEdge&)>& func) noexcept { return this->invoke(func); }

		public:
			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend Path& operator+=(Path& path, T scale) noexcept { for (auto& it : path.edges_) it += scale; return path; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend Path& operator-=(Path& path, T scale) noexcept { for (auto& it : path.edges_) it -= scale; return path; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend Path& operator*=(Path& path, T scale) noexcept { for (auto& it : path.edges_) it *= scale; return path; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend Path& operator/=(Path& path, T scale) noexcept { for (auto& it : path.edges_) it /= scale; return path; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend const Paths& operator+=(const Paths& paths, T scale) noexcept { for (auto& it : paths) *it += scale; return paths; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend const Paths& operator-=(const Paths& paths, T scale) noexcept { for (auto& it : paths) *it -= scale; return paths; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend const Paths& operator*=(const Paths& paths, T scale) noexcept { for (auto& it : paths) *it *= scale; return paths; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend const Paths& operator/=(const Paths& paths, T scale) noexcept { for (auto& it : paths) *it /= scale; return paths; }

		private:
			PathEdges edges_;
		};

		inline std::size_t sum(const Paths& paths) noexcept
		{
			std::size_t sum = 0;
			for (auto& it : paths)
				sum += it->edges().size();

			return sum;
		}

		inline std::size_t max_count(const Paths& paths) noexcept
		{
			std::size_t maxElement = 0;
			for (auto& path : paths)
				maxElement = std::max(maxElement, path->edges().size());

			return maxElement;
		}
	}
}

#endif