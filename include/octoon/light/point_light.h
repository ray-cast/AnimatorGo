#ifndef OCTOON_POINT_LIGHT_H_
#define OCTOON_POINT_LIGHT_H_

#include <octoon/light/light.h>

namespace octoon::light
{
	enum class ShadowMode : std::uint8_t
	{
		ShadowModeNone,
		ShadowModeHard,
		ShadowModeSoft,
		ShadowModeBeginRange = ShadowModeNone,
		ShadowModeEndRange = ShadowModeSoft,
		ShadowModeRangeSize = (ShadowModeEndRange - ShadowModeBeginRange + 1),
	};

	enum class ShadowQuality : std::uint8_t
	{
		ShadowQualityNone,
		ShadowQualityLow,
		ShadowQualityMedium,
		ShadowQualityHigh,
		ShadowQualityVeryHigh,
		ShadowQualityBeginRange = ShadowQualityNone,
		ShadowQualityEndRange = ShadowQualityVeryHigh,
		ShadowQualityRangeSize = (ShadowQualityEndRange - ShadowQualityBeginRange + 1),
	};

	class OCTOON_EXPORT PointLight final : public Light
	{
		OctoonDeclareSubClass(PointLight, Light)
	public:
		PointLight() noexcept;
		virtual ~PointLight() noexcept;

		void setShadowBias(float bias) noexcept;
		void setShadowFactor(float factor) noexcept;
		void setShadowMode(ShadowMode shadowType) noexcept;

		float getShadowBias() const noexcept;
		float getShadowFactor() const noexcept;
		ShadowMode getShadowMode() const noexcept;

		video::RenderObjectPtr clone() const noexcept;

		const std::shared_ptr<camera::Camera>& getCamera() const noexcept;

	private:
		PointLight(const PointLight&) noexcept = delete;
		PointLight& operator=(const PointLight&) noexcept = delete;

	private:
		bool _enableSoftShadow;

		float _shadowBias;
		float _shadowFactor;
		std::shared_ptr<camera::Camera> _shadowCamera;
		ShadowMode _shadowMode;
	};
}

#endif