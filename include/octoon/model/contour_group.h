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

		OCTOON_EXPORT Mesh makeMesh(const Contours& contours, float thickness = 1.0f, bool hollow = false) noexcept;
		OCTOON_EXPORT Mesh makeMesh(const ContourGroup& group, float thickness = 1.0f, bool hollow = false) noexcept;
		OCTOON_EXPORT Mesh makeMesh(const ContourGroups& groups, float thickness = 1.0f, bool hollow = false) noexcept;

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

		inline math::AABB aabb(const ContourGroups& groups) noexcept
		{
			math::AABB aabb;

			for (auto& group : groups)
				for (auto& contour : group->getContours())
					aabb.encapsulate(contour->points());

			return aabb;
		}

		inline math::float2 Twist(const math::float2& OM, float x, float y)
		{
			float xx = math::lerp(OM.x, OM.x + OM.x * OM.y, y);
			float yy = math::lerp(OM.y, OM.y + OM.x * OM.y, x);
			return math::float2(xx, yy);
		}

		inline math::float3 Twist(const math::float3& OM, float x, float y, float z)
		{
			float xx = math::lerp(OM.x, OM.x + OM.x * OM.y, y);
			float yy = math::lerp(OM.y, OM.y + OM.x * OM.y, x);
			float zz = math::lerp(OM.z, OM.z + OM.x * OM.y, z);
			return math::float3(xx, yy, zz);
		}

		// Thanks to : http://tksharpless.net/vedutismo/Pannini/panini.pdf
		inline math::float2 Panini(const math::float2& OM, float x, float y)
		{
			float invLen = math::rsqrt(1.0f + OM.x * OM.x);
			float SinPhi = OM.x * invLen;
			float TanTheta = OM.y * invLen;
			float CosPhi = math::sqrt(1.0f - SinPhi * SinPhi);
			float S = (x + 1.0f) / (x + CosPhi);

			// return S * math::float2(math::lerp(SinPhi, SinPhi / CosPhi, s), TanTheta);
			return S * math::float2(SinPhi, math::lerp(TanTheta, TanTheta / CosPhi, y * 10.0f));
		}
	}
}

#endif