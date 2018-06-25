#ifndef OCTOON_MODEL_PATH_H_
#define OCTOON_MODEL_PATH_H_

#include <octoon/model/path_edge.h>

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

			void setEdge(PathEdges&& edges) noexcept;
			void setEdge(const PathEdge& edge) noexcept;
			void setEdge(const PathEdges& edge) noexcept;
			void setEdge(const math::float3& pt1) noexcept; // point
			void setEdge(const math::float3s& points) noexcept; // points
			void setEdge(const math::float3& pt1, const math::float3& pt2) noexcept; // Line Curve
			void setEdge(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept; // Quadratic Curve
			void setEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

			void addEdge(PathEdges&& edges) noexcept;
			void addEdge(const PathEdge& edge) noexcept;
			void addEdge(const PathEdges& edge) noexcept;
			void addEdge(const math::float3& pt1) noexcept; // point
			void addEdge(const math::float3s& points) noexcept; // points
			void addEdge(const math::float3& pt1, const math::float3& pt2) noexcept; // Line Curve
			void addEdge(const math::float3& pt1, const math::float3& control1, const math::float3& pt2) noexcept; // Quadratic Curve
			void addEdge(const math::float3& pt1, const math::float3& control1, const math::float3& control2, const math::float3& pt2) noexcept; // Cubic Curve

			PathEdge& at(std::size_t index) noexcept;
			const PathEdge& at(std::size_t index) const noexcept;

			PathEdges& edges() noexcept;
			const PathEdges& edges() const noexcept;

			Path& invoke(const std::function<void(Path&)>& func) noexcept;
			Path& invoke(const std::function<void(PathEdge&)>& func) noexcept;
			Path& invoke(const std::function<math::float2(const math::float2&)>& func) noexcept;
			Path& invoke(const std::function<math::float3(const math::float3&)>& func) noexcept;

			Path& operator << (const std::function<void(Path&)>& func) noexcept { return this->invoke(func); }
			Path& operator << (const std::function<void(PathEdge&)>& func) noexcept { return this->invoke(func); }
			Path& operator << (const std::function<math::float2(const math::float2&)>& func) noexcept { return this->invoke(func); }
			Path& operator << (const std::function<math::float3(const math::float3&)>& func) noexcept { return this->invoke(func); }

			PathPtr clone() const noexcept;

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

			friend std::ostream& operator << (std::ostream& os, const Paths& paths) noexcept
			{
				for (auto& it : paths)
					os << it->edges();
				return os;
			}

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