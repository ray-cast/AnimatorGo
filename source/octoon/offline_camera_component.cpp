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
	OfflineCameraComponent::setFarPlane(float farPlane) noexcept
	{
		if (farPlane_ != farPlane)
		{
			if (this->rprCamera_)
			{
				rprCameraSetFarPlane(this->rprCamera_, farPlane);
				this->onFrameDirty();
			}

			farPlane_ = farPlane;
		}
	}

	void
	OfflineCameraComponent::setFocalLength(float focalLength) noexcept
	{
		if (focalLength_ != focalLength)
		{
			if (this->rprCamera_)
			{
				rprCameraSetFocalLength(this->rprCamera_, focalLength);
				this->onFrameDirty();
			}

			focalLength_ = focalLength;
		}
	}

	void
	OfflineCameraComponent::setFocusDistance(float focusDistance) noexcept
	{
		if (focusDistance_ != focusDistance)
		{
			if (this->rprCamera_)
			{
				rprCameraSetFocalLength(this->rprCamera_, focusDistance);
				this->onFrameDirty();
			}

			focusDistance_ = focusDistance;
		}
	}

	void
	OfflineCameraComponent::setFStop(float fStop) noexcept
	{
		if (fStop_ != fStop)
		{
			if (this->rprCamera_)
			{
				rprCameraSetFStop(this->rprCamera_, fStop);
				this->onFrameDirty();
			}

			fStop_ = fStop;
		}
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

			rprCameraSetNearPlane(this->rprCamera_, this->nearPlane_);
			rprCameraSetFarPlane(this->rprCamera_, this->farPlane_);
			rprCameraSetFocalLength(this->rprCamera_, this->focalLength_);
			rprCameraSetFocusDistance(this->rprCamera_, this->focusDistance_);
			rprCameraSetFStop(this->rprCamera_, this->fStop_);
			rprCameraSetMode(this->rprCamera_, RPR_CAMERA_MODE_PERSPECTIVE);

			std::uint32_t w, h;
			feature->getFramebufferScale(w, h);
			rprCameraSetSensorSize(this->rprCamera_, this->focalLength_ + 1, (this->focalLength_ + 1)  / float(w * h));

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
		if (feature && feature->getScene())
		{
			rprSceneSetCamera(feature->getScene(), nullptr);
			rprSceneSetBackgroundImage(feature->getScene(), nullptr);
		}

		if (this->rprCamera_)
		{
			rprObjectDelete(this->rprCamera_);
			this->rprCamera_ = nullptr;
		}

		if (this->rprClearImage_)
		{			
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
			
			this->onFrameDirty();
		}
	}

	void
	OfflineCameraComponent::onLayerChangeAfter() noexcept
	{
	}

	void
	OfflineCameraComponent::onFrameDirty() noexcept
	{
		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
			feature->setFramebufferDirty(true);
	}
}