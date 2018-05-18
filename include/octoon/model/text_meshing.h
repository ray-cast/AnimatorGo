#ifndef OCTOON_MODEL_TEXT_MESHING_H_
#define OCTOON_MODEL_TEXT_MESHING_H_

#include <octoon/model/contour_group.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT TextMeshing final
		{
		public:
			TextMeshing() noexcept;
			TextMeshing(const char* path, std::uint16_t pixelsSize = 12, std::uint16_t bezierSteps = 8, float thickness = 1.0f) noexcept;
			TextMeshing(const std::string& path, std::uint16_t pixelsSize = 12, std::uint16_t bezierSteps = 8, float  thickness = 1.0f) noexcept;
			TextMeshing(TextFilePtr&& font, std::uint16_t pixelsSize = 12, std::uint16_t bezierSteps = 8, float  thickness = 1.0f) noexcept;
			TextMeshing(const TextFilePtr& font, std::uint16_t pixelsSize = 12, std::uint16_t bezierSteps = 8, float thickness = 1.0f) noexcept;
			virtual ~TextMeshing() noexcept;

			void setFont(TextFilePtr&& font) noexcept;
			void setFont(const TextFilePtr& font) noexcept;
			const TextFilePtr& getFont() const noexcept;

			void setPixelsSize(std::uint16_t pixelsSize) noexcept;
			std::uint16_t getPixelsSize() const noexcept;

			void setBezierSteps(std::uint16_t bezierSteps) noexcept;
			std::uint16_t getBezierSteps() const noexcept;

			void setThickness(float thickness) noexcept;
			float getThickness() const noexcept;

			virtual TextMeshingPtr clone() const noexcept;

		private:
			TextMeshing(const TextMeshing&) = delete;
			TextMeshing& operator=(const TextMeshing&) = delete;

		private:
			TextFilePtr font_;

			float thickness_;
			std::wstring string_;
			std::uint16_t bezierSteps_;
			std::uint16_t pixelSize_;
		};

		OCTOON_EXPORT ContourGroups makeTextContours(const std::wstring& string, const TextMeshing& params) noexcept(false);
		OCTOON_EXPORT ContourGroups makeTextContours(const std::wstring& string, const TextMeshing& params) noexcept(false);

		OCTOON_EXPORT Mesh makeText(const std::wstring& string, const TextMeshing& params) noexcept(false);
		OCTOON_EXPORT Mesh makeTextWireframe(const std::wstring& string, const TextMeshing& params) noexcept(false);
	}
}

#endif