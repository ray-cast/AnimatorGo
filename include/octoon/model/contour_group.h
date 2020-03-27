#ifndef OCTOON_MODEL_CONTOUR_GROUP_H_
#define OCTOON_MODEL_CONTOUR_GROUP_H_

#include <octoon/math/math.h>
#include <octoon/mesh/mesh.h>
#include <octoon/model/contour.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT ContourGroup final
		{
		public:
			ContourGroup() noexcept;
			ContourGroup(const PathGroup& pathGroup, std::uint16_t bezierSteps) noexcept;
			explicit ContourGroup(Contours&& contour) noexcept;
			explicit ContourGroup(const Contours& contour) noexcept;
			explicit ContourGroup(const std::initializer_list<ContourPtr>& list) noexcept;
			virtual ~ContourGroup() noexcept;

			void setContours(Contours&& contour) noexcept;
			void setContours(const Contours& contour) noexcept;
			void setContours(const std::initializer_list<ContourPtr>& list) noexcept;

			void addContours(Contours&& contour) noexcept;
			void addContours(const Contours& contour) noexcept;
			void addContours(const std::initializer_list<ContourPtr>& list) noexcept;

			Contour& at(std::size_t index) noexcept;
			const Contour& at(std::size_t index) const noexcept;

			Contours& getContours() noexcept;
			const Contours& getContours() const noexcept;

			Contour& operator[](std::size_t index) noexcept { return this->at(index); }
			const Contour& operator[](std::size_t index) const noexcept { return this->at(index); }

		public:
			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroup& operator+=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) += scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroup& operator-=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) -= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroup& operator*=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) *= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroup& operator/=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) /= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroups& operator+=(ContourGroups& groups, T scale) noexcept { for (auto& it : groups) *it += scale; return groups; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroups& operator-=(ContourGroups& groups, T scale) noexcept { for (auto& it : groups) *it -= scale; return groups; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroups& operator*=(ContourGroups& groups, T scale) noexcept { for (auto& it : groups) *it *= scale; return groups; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value || std::is_same<T, math::detail::Vector3<typename T::value_type>>::value>>
			friend ContourGroups& operator/=(ContourGroups& groups, T scale) noexcept { for (auto& it : groups) *it /= scale; return groups; }

			friend ContourGroup& operator<<(ContourGroup& groups, const std::function<math::float2(const math::float2&)>& func) noexcept
			{
				for (auto& it : groups.getContours())
					*it << func;
				return groups;
			}

			friend ContourGroup& operator<<(ContourGroup& groups, const std::function<math::float3(const math::float3&)>& func) noexcept
			{
				for (auto& it : groups.getContours())
					*it << func;
				return groups;
			}

			friend ContourGroups& operator<<(ContourGroups& groups, const std::function<math::float2(const math::float2&)>& func) noexcept
			{
				for (auto& it : groups)
					*it << func;
				return groups;
			}

			friend ContourGroups& operator<<(ContourGroups& groups, const std::function<math::float3(const math::float3&)>& func) noexcept
			{
				for (auto& it : groups)
					*it << func;
				return groups;
			}

		private:
			ContourGroup(const ContourGroup&) = delete;
			ContourGroup& operator=(const ContourGroup&) = delete;

		private:
			Contours contours_;
		};

		inline math::AABB aabb(const ContourGroup& group) noexcept
		{
			math::AABB aabb;

			for (auto& contour : group.getContours())
				aabb.encapsulate(contour->points());

			return aabb;
		}

		inline math::AABB aabb(const ContourGroups& groups) noexcept
		{
			math::AABB aabb;

			for (auto& group : groups)
				for (auto& contour : group->getContours())
					aabb.encapsulate(contour->points());

			return aabb;
		}

		OCTOON_EXPORT mesh::Mesh makeMesh(const Contours& contours, float thickness, bool hollow) noexcept;
		OCTOON_EXPORT mesh::Mesh makeMesh(const ContourGroup& group, float thickness, bool hollow) noexcept;
		OCTOON_EXPORT mesh::Mesh makeMesh(const ContourGroups& groups, float thickness, bool hollow) noexcept;

		OCTOON_EXPORT mesh::Mesh makeMeshWireframe(const Contours& contours, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT mesh::Mesh makeMeshWireframe(const ContourGroup& group, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT mesh::Mesh makeMeshWireframe(const ContourGroups& groups, float thickness = 1.0f) noexcept;
	}
}

#endif