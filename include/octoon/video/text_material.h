#ifndef OCTOON_TEXT_MATERIAL_H_
#define OCTOON_TEXT_MATERIAL_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace render
	{
		class OCTOON_EXPORT FontMaterial final
		{
		public:
			FontMaterial() noexcept;
			FontMaterial(std::uint32_t width, std::uint32_t height, float lean = 0.0f) noexcept;
			~FontMaterial() noexcept;

			void setWidth(std::uint32_t width) noexcept;
			void setHeight(std::uint32_t height) noexcept;
			void setLean(float lean) noexcept;
			void setExtrude(float extrude) noexcept;

			std::uint32_t getWidth() const noexcept;
			std::uint32_t getHeight() const noexcept;

			float getLean() const noexcept;
			float getExtrude() const noexcept;

		private:
			FontMaterial(const FontMaterial&) = delete;
			FontMaterial& operator=(const FontMaterial&) = delete;

		private:
			float lean_;
			float extrude_;

			std::uint32_t charWidth_;
			std::uint32_t charHeight_;
		};
	}
}

#endif