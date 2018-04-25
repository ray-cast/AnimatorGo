#ifndef OCTOON_TEXT_MESHING_H_
#define OCTOON_TEXT_MESHING_H_

#include <octoon/video/text_contour_group.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT TextMeshing final
		{
		public:
			TextMeshing() noexcept;
			virtual ~TextMeshing() noexcept;

			void setFont(TextFilePtr&& font) noexcept;
			void setFont(const TextFilePtr& font) noexcept;
			const TextFilePtr& getFont() const noexcept;

			void setText(std::wstring&& font) noexcept;
			void setText(const std::wstring& font) noexcept;
			const std::wstring& getText() const noexcept;

			void setBezierSteps(std::uint16_t bezierSteps) noexcept;
			std::uint16_t getBezierSteps() const noexcept;

			virtual TextMeshingPtr clone() const noexcept;

		private:
			void buildContours(const std::wstring& string) noexcept(false);
			TextContourGroupPtr createContours(const void* glyph, float offset, std::uint16_t bezierSteps) noexcept;

		private:
			TextMeshing(const TextMeshing&) = delete;
			TextMeshing& operator=(const TextMeshing&) = delete;

		private:
			std::wstring string_;
			std::uint16_t bezierSteps_;

			TextFilePtr font_;
		};
	}
}

#endif