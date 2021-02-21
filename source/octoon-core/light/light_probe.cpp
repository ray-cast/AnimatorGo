#include <octoon/light/light_probe.h>

namespace octoon
{
	OctoonImplementSubClass(LightProbe, Light, "LightProbe")

	LightProbe::LightProbe() noexcept
	{
	}

	LightProbe::LightProbe(const math::SH9& sh) noexcept
		: sh_(sh)
	{
	}

	LightProbe::~LightProbe() noexcept
	{
	}

	void
	LightProbe::setCoeff(const math::SH9& sh) noexcept
	{
		sh_ = sh;
		this->setDirty(true);
	}

	const math::SH9&
	LightProbe::getCoeff() const noexcept
	{
		return sh_;
	}

	std::shared_ptr<video::RenderObject>
	LightProbe::clone() const noexcept
	{
		auto light = std::make_shared<LightProbe>();
		light->setCoeff(this->getCoeff());
		return light;
	}
}