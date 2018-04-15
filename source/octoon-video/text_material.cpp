#include <octoon/video/text_material.h>

namespace octoon
{
	namespace video
	{
		TextMaterial::TextMaterial() noexcept
			: charWidth_(12)
			, charHeight_(12)
			, lean_(0.0f)
			, extrude_(1.0f)
		{
		}

		TextMaterial::TextMaterial(std::uint32_t width, std::uint32_t height, float lean) noexcept
			: charWidth_(width)
			, charHeight_(height)
			, lean_(lean)
			, extrude_(1.0f)
		{
		}

		TextMaterial::~TextMaterial() noexcept
		{
		}

		void
		TextMaterial::setWidth(std::uint32_t width) noexcept
		{
			charWidth_ = width;
		}

		void
		TextMaterial::setHeight(std::uint32_t height) noexcept
		{
			charHeight_ = height;
		}

		void
		TextMaterial::setLean(float lean) noexcept
		{
			lean_ = lean;
		}

		void
		TextMaterial::setExtrude(float extrude) noexcept
		{
			extrude_ = extrude;
		}

		std::uint32_t
		TextMaterial::getWidth() const noexcept
		{
			return charWidth_;
		}

		std::uint32_t
		TextMaterial::getHeight() const noexcept
		{
			return charHeight_;
		}

		float
		TextMaterial::getLean() const noexcept
		{
			return lean_;
		}

		float
		TextMaterial::getExtrude() const noexcept
		{
			return extrude_;
		}
	}
}