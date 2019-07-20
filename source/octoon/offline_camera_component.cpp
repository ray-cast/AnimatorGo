#include <octoon/offline_camera_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <array>

#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineCameraComponent, GameComponent, "OfflineCamera")

	OfflineCameraComponent::OfflineCameraComponent() noexcept
		: rprCamera_(nullptr)
		, exposure_(1.0f)
		, nearPlane_(0.1f)
		, farPlane_(1000.0f)
		, focalLength_(35.0f)
		, focusDistance_(1.0f)
		, fStop_(0.0f)
		, clearColor_(0.2f, 0.2f, 0.2f, 1.0)
	{
	}

	OfflineCameraComponent::OfflineCameraComponent(float focalLength, float focalDistance, float znear, float zfar) noexcept
		: OfflineCameraComponent()
	{
		this->setFocalLength(focalLength);
		this->setFocusDistance(focalDistance);
		this->setNearPlane(znear);
		this->setFarPlane(zfar);
	}

	OfflineCameraComponent::~OfflineCameraComponent() noexcept
	{
	}

	void
	OfflineCameraComponent::setClearColor(const math::float4& color) noexcept
	{
		this->clearColor_ = color;
	}
	
	const math::float4&
	OfflineCameraComponent::getClearColor() const noexcept
	{
		return this->clearColor_;
	}

	void
	OfflineCameraComponent::setAngularMotion(float angularMotion) noexcept
	{
		angularMotion_ = angularMotion;
	}

	void
	OfflineCameraComponent::setApertureBlades(float apertureBlades) noexcept
	{
		apertureBlades_ = apertureBlades;
	}

	void
	OfflineCameraComponent::setExposure(float exposure) noexcept
	{
		exposure_ = exposure;
	}

	void
	OfflineCameraComponent::setFarPlane(float farPlane) noexcept
	{
		farPlane_ = farPlane;
	}

	void
	OfflineCameraComponent::setFocalLength(float focalLength) noexcept
	{
		focalLength_ = focalLength;
	}

	void
	OfflineCameraComponent::setFocalTilt(float focalTilt) noexcept
	{
		focalTilt_ = focalTilt;
	}

	void
	OfflineCameraComponent::setFocusDistance(float focusDistance) noexcept
	{
		focusDistance_ = focusDistance;
	}

	void
	OfflineCameraComponent::setFStop(float fStop) noexcept
	{
		fStop_ = fStop;
	}

	void
	OfflineCameraComponent::setIPD(float iPD) noexcept
	{
		iPD_ = iPD;
	}

	void
	OfflineCameraComponent::setLensShift(float lensShift) noexcept
	{
		lensShift_ = lensShift;
	}

	void
	OfflineCameraComponent::setLinearMotion(float linearMotion) noexcept
	{
		linearMotion_ = linearMotion;
	}

	void
	OfflineCameraComponent::setMode(float mode) noexcept
	{
		mode_ = mode;
	}

	void
	OfflineCameraComponent::setNearPlane(float nearPlane) noexcept
	{
		nearPlane_ = nearPlane;
	}

	void
	OfflineCameraComponent::setOrthoHeight(float orthoHeight) noexcept
	{
		orthoHeight_ = orthoHeight;
	}

	void
	OfflineCameraComponent::setOrthoWidth(float orthoWidth) noexcept
	{
		orthoWidth_ = orthoWidth;
	}

	void
	OfflineCameraComponent::setSensorSize(float sensorSize) noexcept
	{
		sensorSize_ = sensorSize;
	}

	void
	OfflineCameraComponent::setTiltCorrection(float tiltCorrection) noexcept
	{
		tiltCorrection_ = tiltCorrection;
	}

	float
	OfflineCameraComponent::getAngularMotion() const noexcept
	{
		return this->angularMotion_;
	}

	float
	OfflineCameraComponent::getApertureBlades() const noexcept
	{
		return this->apertureBlades_;
	}

	float
	OfflineCameraComponent::getExposure() const noexcept
	{
		return this->exposure_;
	}

	float
	OfflineCameraComponent::getFarPlane() const noexcept
	{
		return this->farPlane_;
	}

	float
	OfflineCameraComponent::getFocalLength() const noexcept
	{
		return this->focalLength_;
	}

	float
	OfflineCameraComponent::getFocalTilt() const noexcept
	{
		return this->focalTilt_;
	}

	float
	OfflineCameraComponent::getFocusDistance() const noexcept
	{
		return this->focusDistance_;
	}

	float
	OfflineCameraComponent::getFStop() const noexcept
	{
		return this->fStop_;
	}

	float
	OfflineCameraComponent::getIPD() const noexcept
	{
		return this->iPD_;
	}

	float
	OfflineCameraComponent::getLensShift() const noexcept
	{
		return this->lensShift_;
	}

	float
	OfflineCameraComponent::getLinearMotion() const noexcept
	{
		return this->linearMotion_;
	}

	float
	OfflineCameraComponent::getMode() const noexcept
	{
		return this->mode_;
	}

	float
	OfflineCameraComponent::getNearPlane() const noexcept
	{
		return this->nearPlane_;
	}

	float
	OfflineCameraComponent::getOrthoHeight() const noexcept
	{
		return this->orthoHeight_;
	}

	float
	OfflineCameraComponent::getOrthoWidth() const noexcept
	{
		return this->orthoWidth_;
	}

	float
	OfflineCameraComponent::getSensorSize() const noexcept
	{
		return this->sensorSize_;
	}

	float
	OfflineCameraComponent::getTiltCorrection() const noexcept
	{
		return this->tiltCorrection_;
	}

	void
	OfflineCameraComponent::setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
	}

	void
	OfflineCameraComponent::setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
	}

	GameComponentPtr
	OfflineCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineCameraComponent>();
		instance->setName(this->getName());
		instance->setFocalLength(this->getFocalLength());
		instance->setFocusDistance(this->getFocusDistance());
		instance->setFStop(this->getFStop());
		instance->setFarPlane(this->getFarPlane());
		instance->setNearPlane(this->getNearPlane());

		return instance;
	}

	void
	OfflineCameraComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
		{
			rpr_image_format imageFormat = { 3, RPR_COMPONENT_TYPE_FLOAT32 };
			rpr_image_desc imageDesc = { 1, 1, 1, 3, 3 };

			rprContextCreateImage(feature->getContext(), imageFormat, &imageDesc, clearColor_.data(), &this->rprClearImage_);
			rprContextCreateCamera(feature->getContext(), &this->rprCamera_);

			rprCameraSetExposure(this->rprCamera_, this->exposure_);
			rprCameraSetNearPlane(this->rprCamera_, this->nearPlane_);
			rprCameraSetFarPlane(this->rprCamera_, this->farPlane_);
			rprCameraSetFocalLength(this->rprCamera_, this->focalLength_);
			rprCameraSetFocusDistance(this->rprCamera_, this->focusDistance_);
			rprCameraSetFocalTilt(this->rprCamera_, this->focalTilt_);
			rprCameraSetFStop(this->rprCamera_, this->fStop_);
			rprCameraSetMode(this->rprCamera_, RPR_CAMERA_MODE_PERSPECTIVE);

			std::uint32_t w, h;
			feature->getFramebufferScale(w, h);
			rprCameraSetSensorSize(this->rprCamera_, this->focalLength_ + 1, (this->focalLength_ + 1)  / w * (float)h);

			auto transform = this->getComponent<TransformComponent>();
			auto eye = transform->getTranslate();
			auto at = transform->getTranslate() + math::rotate(transform->getQuaternion(), math::float3::UnitZ);
			auto up = math::rotate(transform->getQuaternion(), math::float3::UnitY);
			rprCameraLookAt(this->rprCamera_, eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

			rprSceneSetCamera(feature->getScene(), this->rprCamera_);
			rprSceneSetBackgroundImage(feature->getScene(), this->rprClearImage_);
		}
	}

	void
	OfflineCameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature && this->rprCamera_)
		{
			rprSceneSetCamera(feature->getScene(), nullptr);
			rprObjectDelete(this->rprCamera_);
			this->rprCamera_ = nullptr;
		}

		if (this->rprClearImage_)
		{
			rprSceneSetBackgroundImage(feature->getScene(), nullptr);
			rprObjectDelete(this->rprClearImage_);
			this->rprClearImage_ = nullptr;
		}
	}

	void
	OfflineCameraComponent::onMoveBefore() noexcept
	{
	}

	void
	OfflineCameraComponent::onMoveAfter() noexcept
	{
		if (this->rprCamera_)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto eye = transform->getTranslate();
			auto at = transform->getTranslate() + math::rotate(transform->getQuaternion(), math::float3::UnitZ);
			auto up = math::rotate(transform->getQuaternion(), math::float3::UnitY);
			rprCameraLookAt(this->rprCamera_, eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

			auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}
	}

	void
	OfflineCameraComponent::onLayerChangeAfter() noexcept
	{
	}
}