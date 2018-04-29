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
			ContourGroup(Contours&& contour) noexcept;
			ContourGroup(const Contours& contour) noexcept;
			virtual ~ContourGroup() noexcept;

			void setContours(Contours&& contour) noexcept;
			void setContours(const Contours& contour) noexcept;

			Contour& at(std::size_t index) noexcept;
			const Contour& at(std::size_t index) const noexcept;

			Contours& getContours() noexcept;
			const Contours& getContours() const noexcept;

			std::size_t count() const noexcept;
			std::size_t countOfPoints() const noexcept;

			void normalize(math::float3& center) noexcept;

			ContourGroupPtr clone() const noexcept;

		private:
			ContourGroup(const ContourGroup&) = delete;
			ContourGroup& operator=(const ContourGroup&) = delete;

		private:
			Contours contours_;
		};

		OCTOON_EXPORT Mesh makeMesh(const ContourGroup& group, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMesh(const ContourGroups& groups, float thickness = 1.0f) noexcept;

		OCTOON_EXPORT Mesh makeMeshWireframe(const ContourGroup& group, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT Mesh makeMeshWireframe(const ContourGroups& groups, float thickness = 1.0f) noexcept;
	}
}

#endif