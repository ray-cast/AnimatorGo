#ifndef OCTOON_MODEL_CONTOUR_GROUP_H_
#define OCTOON_MODEL_CONTOUR_GROUP_H_

#include <octoon/math/math.h>
#include <octoon/model/mesh.h>
#include <octoon/model/contour.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT ContourGroup final
		{
		public:
			ContourGroup() noexcept;
			explicit ContourGroup(Contours&& contour) noexcept;
			explicit ContourGroup(const Contours& contour) noexcept;
			virtual ~ContourGroup() noexcept;

			void setContours(Contours&& contour) noexcept;
			void setContours(const Contours& contour) noexcept;

			Contour& at(std::size_t index) noexcept;
			const Contour& at(std::size_t index) const noexcept;

			Contours& getContours() noexcept;
			const Contours& getContours() const noexcept;

			std::size_t count() const noexcept;

			void normalize(math::float3& center) noexcept;

			ContourGroupPtr clone() const noexcept;

		public:
			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend ContourGroup& operator+=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) += scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend ContourGroup& operator-=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) -= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend ContourGroup& operator*=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) *= scale; return contour; }

			template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value | std::is_base_of<T, math::float3>::value>>
			friend ContourGroup& operator/=(ContourGroup& contour, T scale) noexcept { for (auto& it : contour.contours_) (*it) /= scale; return contour; }

		private:
			ContourGroup(const ContourGroup&) = delete;
			ContourGroup& operator=(const ContourGroup&) = delete;

		private:
			Contours contours_;
		};

		OCTOON_EXPORT Mesh makeMesh(const Contours& contours, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMesh(const ContourGroup& group, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMesh(const ContourGroups& groups, float thickness = 1.0f) noexcept;

		OCTOON_EXPORT Mesh makeMeshWireframe(const Contours& contours, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMeshWireframe(const ContourGroup& group, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMeshWireframe(const ContourGroups& groups, float thickness = 1.0f) noexcept;

		inline math::AABB aabb(const ContourGroup& group) noexcept
		{
			math::AABB aabb;

			for (auto& contour : group.getContours())
				aabb.encapsulate(contour->points());

			return aabb;
		}
	}
}

#endif