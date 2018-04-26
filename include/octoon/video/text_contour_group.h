#ifndef OCTOON_TEXT_CONTOUR_GROUP_H_
#define OCTOON_TEXT_CONTOUR_GROUP_H_

#include <octoon/math/math.h>
#include <octoon/model/mesh.h>
#include <octoon/video/text_contour.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT TextContourGroup final
		{
		public:
			TextContourGroup() noexcept;
			TextContourGroup(TextContours&& contour) noexcept;
			virtual ~TextContourGroup() noexcept;

			void setContours(TextContours&& contour) noexcept;
			void setContours(const TextContours& contour) noexcept;

			TextContour& at(std::size_t index) noexcept;
			const TextContour& at(std::size_t index) const noexcept;

			TextContours& getContours() noexcept;
			const TextContours& getContours() const noexcept;

			std::size_t count() const noexcept;
			std::size_t countOfPoints() const noexcept;

			void clear() noexcept;
			void scale(float scaler) noexcept;

			void normalize(math::float3& center) noexcept;

			void buildMeshes(model::Mesh& mesh) noexcept;

			TextContourGroupPtr clone() const noexcept;

		private:
			TextContourGroup(const TextContourGroup&) = delete;
			TextContourGroup& operator=(const TextContourGroup&) = delete;

		private:
			TextContours contours_;
		};
	}
}

#endif