#ifndef OCTOON_MODEL_TEXT_MESHING_H_
#define OCTOON_MODEL_TEXT_MESHING_H_

#include <octoon/model/path_group.h>
#include <octoon/model/contour_group.h>
#include <octoon/model/font.h>

namespace octoon::font
{
	enum class TextAlign
	{
		Left,
		Right,
		Middle
	};

	class OCTOON_EXPORT TextMeshing final
	{
	public:
		TextMeshing() noexcept;
		TextMeshing(const char* path, std::uint16_t pixelsSize = 12) noexcept;
		TextMeshing(const std::string& path, std::uint16_t pixelsSize = 12) noexcept;
		TextMeshing(std::shared_ptr<Font>&& font, std::uint16_t pixelsSize = 12) noexcept;
		TextMeshing(const std::shared_ptr<Font>& font, std::uint16_t pixelsSize = 12) noexcept;
		virtual ~TextMeshing() noexcept;

		void setFont(std::shared_ptr<Font>&& font) noexcept;
		void setFont(const std::shared_ptr<Font>& font) noexcept;
		const std::shared_ptr<Font>& getFont() const noexcept;

		void setPixelsSize(std::uint16_t pixelsSize) noexcept;
		std::uint16_t getPixelsSize() const noexcept;

		virtual std::shared_ptr<TextMeshing> clone() const noexcept;

	private:
		TextMeshing(const TextMeshing&) = delete;
		TextMeshing& operator=(const TextMeshing&) = delete;

	private:
		std::shared_ptr<Font> font_;
		std::uint16_t pixelSize_;
	};

	OCTOON_EXPORT PathGroups makeTextPaths(const std::wstring& string, const TextMeshing& params) noexcept(false);

	OCTOON_EXPORT Contours makeTextContours(const Paths& paths, std::uint16_t bezierSteps = 8) noexcept(false);
	OCTOON_EXPORT ContourGroups makeTextContours(const PathGroups& paths, std::uint16_t bezierSteps = 8) noexcept(false);
	OCTOON_EXPORT ContourGroups makeTextContours(const std::wstring& string, const TextMeshing& params, std::uint16_t bezierSteps = 8, TextAlign align = TextAlign::Left) noexcept(false);

	OCTOON_EXPORT Mesh makeText(const std::wstring& string, const TextMeshing& params, float thickness = 1.0f, std::uint16_t bezierSteps = 8) noexcept(false);
	OCTOON_EXPORT Mesh makeTextWireframe(const std::wstring& string, const TextMeshing& params, float thickness = 1.0f, std::uint16_t bezierSteps = 8) noexcept(false);
}

#endif