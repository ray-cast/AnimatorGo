#include <octoon/offline_point_light_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflinePointLightComponent, OfflineLightComponent, "OfflineSpotLight")

	OfflinePointLightComponent::OfflinePointLightComponent() noexcept
		: rprLight_(nullptr)
		, color_(math::float3::One)
	{
	}

	OfflinePointLightComponent::~OfflinePointLightComponent() noexcept
	{
	}

	void
	OfflinePointLightComponent::setIntensity(float value) noexcept
	{
		if (intensity_ != value)
		{
			if (this->rprLight_)
			{
				auto intensity = this->getIntensity();
				if (RPR_SUCCESS != rprPointLightSetRadiantPower3f(this->rprLight_, color_.x * intensity, color_.y * intensity, color_.z * intensity))
					return;

				auto feature = this->tryGetFeature<OfflineFeature>();
				if (feature)
					feature->setFramebufferDirty(true);
			}

			OfflineLightComponent::setIntensity(value);
		}
	}

	void
	OfflinePointLightComponent::setColor(const math::float3& value) noexcept
	{
		if (color_ != value)
		{
			if (this->rprLight_)
			{
				auto intensity = this->getIntensity();
				if (RPR_SUCCESS != rprPointLightSetRadiantPower3f(this->rprLight_, value.x * intensity, value.y * intensity, value.z * intensity))
					return;

				auto feature = this->tryGetFeature<OfflineFeature>();
				if (feature)
					feature->setFramebufferDirty(true);
			}

			OfflineLightComponent::setColor(value);
		}
	}

	GameComponentPtr
	OfflinePointLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflinePointLightComponent>();
		instance->setName(this->getName());
		instance->setColor(this->getColor());
		instance->setIntensity(this->getIntensity());
		return instance;
	}

	void
	OfflinePointLightComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			auto transform = this->getComponent<TransformComponent>();

			if (RPR_SUCCESS != rprContextCreatePointLight(feature->getContext(), &this->rprLight_))
				return;
			if (RPR_SUCCESS != rprPointLightSetRadiantPower3f(this->rprLight_, color_.x, color_.y, color_.z))
				return;
			if (RPR_SUCCESS != rprLightSetTransform(this->rprLight_, false, transform->getTransform().ptr()))
				return;
			if (RPR_SUCCESS != rprSceneAttachLight(feature->getScene(), this->rprLight_))
				return;

			feature->setFramebufferDirty(true);
		}
	}

	void
	OfflinePointLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getFeature<OfflineFeature>();
		if (feature && this->rprLight_)
		{
			feature->setFramebufferDirty(true);
			rprSceneDetachLight(feature->getScene(), this->rprLight_);
		}

		if (this->rprLight_)
		{
			rprObjectDelete(this->rprLight_);
			this->rprLight_ = nullptr;
		}
	}

	void
	OfflinePointLightComponent::onMoveAfter() noexcept
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
	OfflinePointLightComponent::onLayerChangeAfter() noexcept
	{
		if (RPR_SUCCESS != rprLightSetGroupId(this->rprLight_, this->getGameObject()->getLayer()))
			return;
	}
}