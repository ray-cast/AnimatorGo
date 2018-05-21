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

		inline math::float2 Twist(const math::float2& pt, float x, float y)
		{
			float xx = math::lerp(pt.x, pt.x + pt.x * pt.y, y);
			float yy = math::lerp(pt.y, pt.y + pt.x * pt.y, x);
			return math::float2(xx, yy);
		}

		inline math::float3 Twist(const math::float3& pt, float x, float y, float z)
		{
			float xx = math::lerp(pt.x, pt.x + pt.x * pt.y, y);
			float yy = math::lerp(pt.y, pt.y + pt.x * pt.y, x);
			float zz = math::lerp(pt.z, pt.z + pt.x * pt.y, z);
			return math::float3(xx, yy, zz);
		}

		inline math::float2 fan(const math::float2& pt, float x, float y)
		{
			float unorm = math::saturate(math::snorm2unorm(pt.y));
			float weight = math::cos(pt.x * math::PI * 0.5f) * 7.0f;
			float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.3f - abs(pt.x)), abs(x));
			float yy = math::lerp(pt.y, pt.y + weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 LowCove(const math::float2& pt, float x, float y)
		{
			float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
			float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * 6.8f;
			float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - abs(pt.x)) * 0.3f, abs(x));
			float yy = math::lerp(pt.y, pt.y - weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 HighCove(const math::float2& pt, float x, float y)
		{
			float unorm = math::saturate(math::snorm2unorm(pt.y));
			float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * 7.0f;
			float xx = math::lerp(pt.x, pt.x + pt.x * unorm * (1.0f - abs(pt.x)) * 0.3f, abs(x));
			float yy = math::lerp(pt.y, pt.y + weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 Cove(const math::float2& pt, float x, float y)
		{
			float weight = math::cos(pt.x * math::PI * 0.5f) * 8.0f;
			float xx = math::lerp(pt.x, pt.x + pt.x * (1.0f - math::abs(pt.x)) * 0.5f, abs(x));
			float yy = math::lerp(pt.y, pt.y + weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 Bulege(const math::float2& pt, float x, float y)
		{
			float weight = math::cos(pt.x * math::PI * 0.5f) * math::length(pt) * 8.0f;
			float xx = math::lerp(pt.x, pt.x + pt.x * weight, y);
			float yy = math::lerp(pt.y, pt.y + pt.y * weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 BulegeHigh(const math::float2& pt, float x, float y)
		{
			float unorm = math::saturate(math::snorm2unorm(pt.y));
			float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * unorm * 6.8f;
			float xx = math::lerp(pt.x, pt.x + pt.x * unorm * 0.3f, abs(x));
			float yy = math::lerp(pt.y, pt.y + weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 BulegeLow(const math::float2& pt, float x, float y)
		{
			float unorm = 1.0f - math::saturate(math::snorm2unorm(pt.y));
			float weight = math::cos(pt.x * math::PI * 0.5f) * unorm * unorm * 6.8f;
			float xx = math::lerp(pt.x, pt.x + pt.x * unorm * 0.3f, abs(x));
			float yy = math::lerp(pt.y, pt.y - weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 Fish(const math::float2& pt, float x, float y)
		{
			float xx = math::lerp(pt.x, pt.x + pt.x * math::sin(pt.y * math::PI), y);
			float yy = math::lerp(pt.y, pt.y + pt.y * math::sin(pt.x * -math::PI), x);
			return math::float2(xx, yy);
		}

		inline math::float2 Flag(const math::float2& pt, float x, float y)
		{
			float xx = math::lerp(pt.x, pt.x, y);
			float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI), x);
			return math::float2(xx, yy);
		}

		inline math::float2 Wave(const math::float2& pt, float x, float y)
		{
			float xx = math::lerp(pt.x, pt.x, y);
			float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI) * (1.0f - abs(pt.y)), x);
			return math::float2(xx, yy);
		}

		inline math::float2 Slope(const math::float2& pt, float x, float y)
		{
			float xx = math::lerp(pt.x, pt.x + math::sin(pt.y * math::PI * 0.5f) - math::sin(math::PI * 0.5f), y * 2.0f);
			float yy = math::lerp(pt.y, pt.y + math::sin(pt.x * math::PI * 0.5f) - math::sin(math::PI * 0.5f), x * 2.0f);
			return math::float2(xx, yy);
		}

		inline math::float2 FishEye(const math::float2& pt, float ratio, float x, float y)
		{
			float weight = 1.1f - math::length(pt);
			float xx = math::lerp(pt.x, pt.x + pt.x * weight, x);
			float yy = math::lerp(pt.y, pt.y + pt.y * weight, x / ratio);
			return math::float2(xx, yy);
		}

		inline math::float2 ExpandOut(const math::float2& pt, float x, float y)
		{
			float weight = 1.5f - math::length(pt);
			float xx = math::lerp(pt.x, pt.x + pt.x * weight, x);
			float yy = math::lerp(pt.y, pt.y + pt.y * weight, x);
			return math::float2(xx, yy);
		}

		inline math::float2 ExpandIn(const math::float2& pt, float x, float y)
		{
			float weight = 1.5f - math::length(pt);
			float xx = math::lerp(pt.x, pt.x - pt.x * weight, x * 0.5f);
			float yy = math::lerp(pt.y, pt.y + pt.y * weight, x * 0.5f);
			return math::float2(xx, yy);
		}

		inline math::float2 Spin(const math::float2& pt, float ratio, float x, float y)
		{
			math::float2 spin;
			math::sinCos(&spin.x, &spin.y, x * (1.0f - math::length(pt)) * 0.5f);

			float xx = math::lerp(pt.x, pt.x + pt.y * spin.x, x);
			float yy = math::lerp(pt.y, pt.x + pt.y * spin.y, x / ratio);

			return math::float2(xx, yy);
		}

		inline math::float2 Panini(const math::float2& pt, float x, float y)
		{
			// Thanks to : http://tksharpless.net/vedutismo/Pannini/panini.pdf
			float invLen = math::rsqrt(1.0f + pt.x * pt.x);
			float SinPhi = pt.x * invLen;
			float TanTheta = pt.y * invLen;
			float CosPhi = math::sqrt(1.0f - SinPhi * SinPhi);
			float S = (x + 1.0f) / (x + CosPhi);

			// return S * math::float2(math::lerp(SinPhi, SinPhi / CosPhi, s), TanTheta);
			return S * math::float2(SinPhi, math::lerp(TanTheta, TanTheta / CosPhi, y * 10.0f));
		}
	}
}

#endif