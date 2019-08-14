#include <octoon/offline_spot_light_component.h>
#include <octoon/offline_feature.h>
#include <octoon/game_scene.h>
#include <octoon/transform_component.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineSpotLightComponent, OfflineLightComponent, "OfflineSpotLight")

	OfflineSpotLightComponent::OfflineSpotLightComponent() noexcept
		: rprLight_(nullptr)
		, color_(math::float3::One)
		, innerAngle_(30.0f)
		, outerAngle_(60.0f)
	{
	}

	OfflineSpotLightComponent::~OfflineSpotLightComponent() noexcept
	{
	}

	void
	OfflineSpotLightComponent::setIntensity(float value) noexcept
	{
		if (this->rprLight_)
		{
			rprEnvironmentLightSetIntensityScale(this->rprLight_, value);

			auto feature = this->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}

		OfflineLightComponent::setIntensity(value);
	}

	void
	OfflineSpotLightComponent::setColor(const math::float3& value) noexcept
	{
		if (this->rprLight_)
		{
			auto intensity = this->getIntensity();
			if (RPR_SUCCESS != rprDirectionalLightSetRadiantPower3f(this->rprLight_, value.x * intensity, value.y * intensity, value.z * intensity))
				return;

			auto feature = this->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}

		OfflineLightComponent::setColor(value);
	}

	void
	OfflineSpotLightComponent::setInnerAngle(float value) noexcept
	{
		if (innerAngle_ != value)
		{
			if (this->rprLight_)
			{
				rprSpotLightSetConeShape(this->rprLight_, value, outerAngle_);

				auto feature = this->getGameScene()->getFeature<OfflineFeature>();
				if (feature)
					feature->setFramebufferDirty(true);

			}

			innerAngle_ = value;
		}
	}

	void 
	OfflineSpotLightComponent::setOuterAngle(float value) noexcept
	{
		if (outerAngle_ != value)
		{
			if (this->rprLight_)
			{
				rprSpotLightSetConeShape(this->rprLight_, innerAngle_, value);

				auto feature = this->getGameScene()->getFeature<OfflineFeature>();
				if (feature)
					feature->setFramebufferDirty(true);

			}

			outerAngle_ = value;
		}
	}

	float
	OfflineSpotLightComponent::getInnerAngle() const noexcept
	{
		return innerAngle_;
	}

	float
	OfflineSpotLightComponent::getOuterAngle() const noexcept
	{
		return outerAngle_;
	}

	GameComponentPtr
	OfflineSpotLightComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineSpotLightComponent>();
		instance->setName(this->getName());

		return instance;
	}

	void
	OfflineSpotLightComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
		{
			auto transform = this->getComponent<TransformComponent>();

			if (RPR_SUCCESS != rprContextCreateSpotLight(feature->getContext(), &this->rprLight_))
				return;
			if (RPR_SUCCESS != rprSpotLightSetConeShape(this->rprLight_, innerAngle_, outerAngle_))
				return;
			if (RPR_SUCCESS != rprSpotLightSetRadiantPower3f(this->rprLight_, color_.x, color_.y, color_.z))
				return;
			if (RPR_SUCCESS != rprLightSetTransform(this->rprLight_, false, transform->getTransform().ptr()))
				return;
			if (RPR_SUCCESS != rprSceneAttachLight(feature->getScene(), this->rprLight_))
				return;
		}
	}

	void
	OfflineSpotLightComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getGameScene()->getFeature<OfflineFeature>();
		if (feature && this->rprLight_)
			rprSceneDetachLight(feature->getScene(), this->rprLight_);

		if (feature && this->rprLight_)
		{
			rprObjectDelete(this->rprLight_);
			this->rprLight_ = nullptr;
		}
	}

	void
	OfflineSpotLightComponent::onMoveAfter() noexcept
	{
		if (this->rprLight_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rprLightSetTransform(this->rprLight_, false, transform->getTransform().ptr());

			auto feature = this->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}
	}

	void
	OfflineSpotLightComponent::onLayerChangeAfter() noexcept
	{
		if (RPR_SUCCESS != rprLightSetGroupId(this->rprLight_, this->getGameObject()->getLayer()))
			return;
	}
}