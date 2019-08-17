#include <octoon/offline_directional_light_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineDirectionalLightComponent, OfflineLightComponent, "OfflineDirectionalLight")

	OfflineDirectionalLightComponent::OfflineDirectionalLightComponent() noexcept
		: rprLight_(nullptr)
		, color_(math::float3::One)
	{
	}

	OfflineDirectionalLightComponent::~OfflineDirectionalLightComponent() noexcept
	{
	}

	void
	OfflineDirectionalLightComponent::setIntensity(float value) noexcept
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

	void
	OfflineDirectionalLightComponent::setColor(const math::float3& value) noexcept
	{
		if (this->rprLight_)
		{
			auto intensity = this->getIntensity();
			if (RPR_SUCCESS != rprDirectionalLightSetRadiantPower3f(this->rprLight_, value.x * intensity, value.y * intensity, value.z * intensity))
				return;

			auto feature = this->tryGetFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}

		OfflineLightComponent::setColor(value);
	}

	GameComponentPtr
	OfflineDirectionalLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineDirectionalLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	OfflineDirectionalLightComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			auto transform = this->getComponent<TransformComponent>();

			if (RPR_SUCCESS != rprContextCreateDirectionalLight(feature->getContext(), &this->rprLight_))
				return;
			if (RPR_SUCCESS != rprDirectionalLightSetRadiantPower3f(this->rprLight_, color_.x, color_.y, color_.z))
				return;
			if (RPR_SUCCESS != rprLightSetTransform(this->rprLight_, false, transform->getTransform().ptr()))
				return;
			if (RPR_SUCCESS != rprSceneAttachLight(feature->getScene(), this->rprLight_))
				return;
		}
	}

	void
	OfflineDirectionalLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getFeature<OfflineFeature>();
		if (feature && this->rprLight_)
			rprSceneDetachLight(feature->getScene(), this->rprLight_);

		if (feature && this->rprLight_)
		{
			rprObjectDelete(this->rprLight_);
			this->rprLight_ = nullptr;
		}
	}

	void
	OfflineDirectionalLightComponent::onMoveAfter() noexcept
	{
		if (this->rprLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rprLightSetTransform(this->rprLight_, false, transform->getTransform().ptr());

			auto feature = this->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}
	}

	void
	OfflineDirectionalLightComponent::onLayerChangeAfter() noexcept
	{
		if (RPR_SUCCESS != rprLightSetGroupId(this->rprLight_, this->getGameObject()->getLayer()))
			return;
	}
}