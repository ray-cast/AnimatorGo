#include <octoon/light/point_light.h>
#include <octoon/camera/perspective_camera.h>

namespace octoon::light
{
	OctoonImplementSubClass(PointLight, Light, "PointLight")

	PointLight::PointLight() noexcept
		: shadowEnable_(false)
		, shadowBias_(0.1f)
		, shadowRadius_(1.0f)
		, shadowSize_(512, 512)
	{
		auto shadowCamera = std::make_shared<camera::PerspectiveCamera>();
		shadowCamera->setAperture(90.0f);
		shadowCamera->setNear(0.1f);
		shadowCamera->setSensorSize(math::float2::One);
		shadowCamera->setOwnerListener(this);
	}

	PointLight::~PointLight() noexcept
	{
	}

	void
	PointLight::setShadowEnable(bool enable) noexcept
	{
		if (this->shadowEnable_ != enable)
		{
			if (this->shadowCamera_ && enable)
				this->shadowCamera_->setupFramebuffers(512, 512, 0, hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat::D32_SFLOAT);
			this->setDirty(true);
			this->shadowEnable_ = enable;
		}
	}

	bool
	PointLight::getShadowEnable() const noexcept
	{
		return this->shadowEnable_;
	}

	void
	PointLight::setShadowBias(float bias) noexcept
	{
		this->setDirty(true);
		shadowBias_ = bias;
	}

	float
	PointLight::getShadowBias() const noexcept
	{
		return shadowBias_;
	}

	void
	PointLight::setShadowRadius(float radius) noexcept
	{
		this->setDirty(true);
		this->shadowRadius_ = radius;
	}
	
	float
	PointLight::getShadowRadius() const noexcept
	{
		return this->shadowRadius_;
	}

	void
	PointLight::setShadowMapSize(const math::uint2& size) noexcept
	{
		if (this->shadowSize_ != size)
		{
			if (this->shadowCamera_ && this->shadowEnable_)
				this->shadowCamera_->setupFramebuffers(size.x, size.y, 0, hal::GraphicsFormat::R8G8B8A8UNorm, hal::GraphicsFormat::D32_SFLOAT);
			this->setDirty(true);
			this->shadowSize_ = size;
		}
	}

	const math::uint2&
	PointLight::getShadowMapSize() const noexcept
	{
		return this->shadowSize_;
	}

	const std::shared_ptr<camera::Camera>&
	PointLight::getCamera() const noexcept
	{
		return this->shadowCamera_;
	}

	std::shared_ptr<video::RenderObject>
	PointLight::clone() const noexcept
	{
		auto light = std::make_shared<PointLight>();

		return light;
	}
}