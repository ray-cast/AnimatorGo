#include <octoon/offline_environment_light_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineEnvironmentLightComponent, OfflineLightComponent, "OfflineEnvironmentLight")

	OfflineEnvironmentLightComponent::OfflineEnvironmentLightComponent() noexcept
		: rprLight_(nullptr)
		, rprImage_(nullptr)
		, color_(math::float3::One)
	{
	}

	OfflineEnvironmentLightComponent::~OfflineEnvironmentLightComponent() noexcept
	{
	}

	void
	OfflineEnvironmentLightComponent::setIntensity(float value) noexcept
	{
		if (this->rprLight_)
		{
			rprEnvironmentLightSetIntensityScale(this->rprLight_, value);

			auto feature = this->tryGetFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}

		OfflineLightComponent::setIntensity(value);
	}

	GameComponentPtr
	OfflineEnvironmentLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineEnvironmentLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	OfflineEnvironmentLightComponent::onActivate() noexcept
	{
		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			rpr_image_format format = { 3, RPR_COMPONENT_TYPE_FLOAT32 };
			rpr_image_desc desc = { 1, 1, 1, 3, 3 };

			if (RPR_SUCCESS != rprContextCreateImage(feature->getContext(), format, &desc, color_.ptr(), &this->rprImage_))
				return;
			if (RPR_SUCCESS != rprContextCreateEnvironmentLight(feature->getContext(), &this->rprLight_))
				return;
			if (RPR_SUCCESS != rprEnvironmentLightSetImage(this->rprLight_, this->rprImage_))
				return;
			if (RPR_SUCCESS != rprEnvironmentLightSetIntensityScale(this->rprLight_, this->getIntensity()))
				return;
			if (RPR_SUCCESS != rprSceneAttachLight(feature->getScene(), this->rprLight_))
				return;
		}
	}

	void
	OfflineEnvironmentLightComponent::onDeactivate() noexcept
	{
		auto feature = this->getFeature<OfflineFeature>();
		if (feature && this->rprLight_)
			rprSceneDetachLight(feature->getScene(), this->rprLight_);

		if (feature && this->rprLight_)
		{
			rprObjectDelete(this->rprLight_);
			this->rprLight_ = nullptr;
		}

		if (this->rprImage_)
		{
			rprObjectDelete(this->rprImage_);
			this->rprImage_ = nullptr;
		}
	}

	void
	OfflineEnvironmentLightComponent::onLayerChangeAfter() noexcept
	{
		if (RPR_SUCCESS != rprLightSetGroupId(this->rprLight_, this->getGameObject()->getLayer()))
			return;
	}
}