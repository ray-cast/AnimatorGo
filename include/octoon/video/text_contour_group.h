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
			TextContourGroup(const TextContours& contour) noexcept;
			virtual ~TextContourGroup() noexcept;

			void setContours(TextContours&& contour) noexcept;
			void setContours(const TextContours& contour) noexcept;

			TextContour& at(std::size_t index) noexcept;
			const TextContour& at(std::size_t index) const noexcept;

			TextContours& getContours() noexcept;
			const TextContours& getContours() const noexcept;

			std::size_t count() const noexcept;
			std::size_t countOfPoints() const noexcept;

			void normalize(math::float3& center) noexcept;

			TextContourGroupPtr clone() const noexcept;

		private:
			TextContourGroup(const TextContourGroup&) = delete;
			TextContourGroup& operator=(const TextContourGroup&) = delete;

		private:
			TextContours contours_;
		};

		OCTOON_EXPORT model::Mesh makeText(const TextContourGroup& group) noexcept;
		OCTOON_EXPORT model::Mesh makeText(const TextContourGroups& groups) noexcept;
	}
}

#endif