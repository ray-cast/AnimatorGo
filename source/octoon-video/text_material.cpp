#include <octoon/video/text_material.h>

namespace octoon
{
	namespace render
	{
		FontMaterial::FontMaterial() noexcept
			: charWidth_(12)
			, charHeight_(12)
			, lean_(0.0f)
			, extrude_(1.0f)
		{
		}

		FontMaterial::FontMaterial(std::uint32_t width, std::uint32_t height, float lean) noexcept
			: charWidth_(width)
			, charHeight_(height)
			, lean_(lean)
			, extrude_(1.0f)
		{
		}

		FontMaterial::~FontMaterial() noexcept
		{
		}

		void
		FontMaterial::setWidth(std::uint32_t width) noexcept
		{
			charWidth_ = width;
		}

		void
		FontMaterial::setHeight(std::uint32_t height) noexcept
		{
			charHeight_ = height;
		}

		void
		FontMaterial::setLean(float lean) noexcept
		{
			lean_ = lean;
		}

		void
		FontMaterial::setExtrude(float extrude) noexcept
		{
			extrude_ = extrude;
		}

		std::uint32_t
		FontMaterial::getWidth() const noexcept
		{
			return charWidth_;
		}

		std::uint32_t
		FontMaterial::getHeight() const noexcept
		{
			return charHeight_;
		}

		float
		FontMaterial::getLean() const noexcept
		{
			return lean_;
		}

		float
		FontMaterial::getExtrude() const noexcept
		{
			return extrude_;
		}
	}
}