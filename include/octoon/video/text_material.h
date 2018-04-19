#ifndef OCTOON_TEXT_MATERIAL_H_
#define OCTOON_TEXT_MATERIAL_H_

#include <octoon/video/material.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT TextMaterial final : public Material
		{
		public:
			TextMaterial() noexcept;
			TextMaterial(float lean) noexcept;
			~TextMaterial() noexcept;

			void setLean(float lean) noexcept;
			void setExtrude(float extrude) noexcept;
			void setTextColor(TextColor::Type which, const math::float3& colors) noexcept;
			void setShaingMode(TextShaingMode mode) noexcept;
			void setTranslate(const math::float3& translate) noexcept;

			float getLean() const noexcept;
			float getExtrude() const noexcept;
			TextShaingMode getShaingMode() const noexcept;
			const math::float3& getTranslate() const noexcept;

			const math::float3& getTextColor(TextColor::Type which) const noexcept;

			MaterialPtr clone() const noexcept override;

		private:
			TextMaterial(const TextMaterial&) = delete;
			TextMaterial& operator=(const TextMaterial&) = delete;

		private:
			TextShaingMode mode_;

			math::float1 lean_;
			math::float1 extrude_;
			math::float3 translate_;
			math::float3 colors_[TextColor::RangeSize_];
		};
	}
}

#endif