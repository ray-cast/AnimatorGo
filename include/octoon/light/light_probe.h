#ifndef OCTOON_LIGHT_PROBE_H_
#define OCTOON_LIGHT_PROBE_H_

#include <octoon/light/light.h>

namespace octoon::light
{
	class OCTOON_EXPORT LightProbe final : public Light
	{
		OctoonDeclareSubClass(LightProbe, Light)
	public:
		LightProbe() noexcept;
		LightProbe(const math::SH9& sh) noexcept;
		virtual ~LightProbe() noexcept;

		void setCoeff(const math::SH9& sh) noexcept;
		const math::SH9& getCoeff() const noexcept;

		std::shared_ptr<video::RenderObject> clone() const noexcept;

	private:
		LightProbe(const LightProbe&) noexcept = delete;
		LightProbe& operator=(const LightProbe&) noexcept = delete;

	private:
		math::SH9 sh_;
	};
}

#endif