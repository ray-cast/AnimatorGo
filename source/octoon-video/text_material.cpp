#include <octoon/video/text_material.h>

namespace octoon
{
	namespace video
	{
		TextMaterial::TextMaterial() noexcept
			: lean_(0.0f)
			, extrude_(1.0f)
			, translate_(math::float3::Zero)
			, mode_(TextShaingMode::PureColor)
		{
			colors_[TextColor::FrontColor] = math::float3::One;
			colors_[TextColor::SideColor] = math::float3::Zero;
		}

		TextMaterial::TextMaterial(float lean) noexcept
			: lean_(lean)
			, extrude_(1.0f)
		{
		}

		TextMaterial::~TextMaterial() noexcept
		{
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

		void
		TextMaterial::setTextColor(TextColor::Type which, const math::float3& colors) noexcept
		{
			assert(which >= TextColor::BeginRange_ && which <= TextColor::EndRange_);
			colors_[which] = colors;
		}

		void
		TextMaterial::setShaingMode(TextShaingMode mode) noexcept
		{
			mode_ = mode;
		}

		void
		TextMaterial::setTranslate(const math::float3& translate) noexcept
		{
			translate_ = translate;
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

		TextShaingMode
		TextMaterial::getShaingMode() const noexcept
		{
			return mode_;
		}

		const math::float3&
		TextMaterial::getTranslate() const noexcept
		{
			return translate_;
		}

		const math::float3&
		TextMaterial::getTextColor(TextColor::Type which) const noexcept
		{
			assert(which >= TextColor::BeginRange_ && which <= TextColor::EndRange_);
			return colors_[which];
		}

		MaterialPtr
		TextMaterial::clone() const noexcept
		{
			auto instance = std::make_shared<TextMaterial>();
			instance->setShaingMode(this->getShaingMode());
			instance->setExtrude(this->getExtrude());
			instance->setLean(this->getLean());
			instance->setTextColor(TextColor::FrontColor, this->getTextColor(TextColor::FrontColor));
			instance->setTextColor(TextColor::SideColor, this->getTextColor(TextColor::SideColor));

			return instance;
		}
	}
}