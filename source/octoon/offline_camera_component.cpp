#include <octoon/offline_camera_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <octoon/image/image.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineCameraComponent, GameComponent, "OfflineCamera")

	OfflineCameraComponent::OfflineCameraComponent() noexcept
		: rprCamera_(nullptr)
		, nearPlane_(0.1f)
		, farPlane_(1000.0f)
		, focalLength_(35.0f)
		, filmSize_(36.0f)
		, focusDistance_(1.0f)
		, fStop_(0.0f)
		, clearColor_(0.8f, 0.9f, 1.0f, 1.0)
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
	OfflineCameraComponent::setAperture(float aperture) noexcept
	{
		if (aperture_ != aperture)
		{
			auto ratio = std::tan(math::radians(aperture) * 0.5f) * 2.0f;
			auto length = filmSize_ / ratio;
			this->setFocalLength(length);
			aperture_ = aperture;
		}
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
	OfflineCameraComponent::getAperture() const noexcept
	{
		return this->aperture_;
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
	OfflineCameraComponent::setBgImage(const std::string& path) noexcept
	{
		if (path_ != path)
		{
			if (this->rprCamera_ )
			{
				if (this->rprClearImage_)
					rprObjectDelete(rprClearImage_);

				auto feature = this->tryGetFeature<OfflineFeature>();
				if (feature)
				{
					if (!path_.empty())
					{
						this->rprClearImage_ = this->createImage(path);
						rprSceneSetBackgroundImage(feature->getScene(), this->rprClearImage_);
					}
					else
					{
						rprSceneSetBackgroundImage(feature->getScene(), nullptr);
					}

					feature->setFramebufferDirty(true);
				}
			}

			path_ = path;
		}
	}
	
	const std::string&
	OfflineCameraComponent::getBgImage() const noexcept
	{
		return path_;
	}

	hal::GraphicsFramebufferPtr
	OfflineCameraComponent::getFramebuffer() const noexcept
	{
		auto feature = this->tryGetFeature<OfflineFeature>();
		if (feature)
			return feature->getColorFramebuffer();
		return nullptr;
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
		this->addMessageListener("Camera:fov", std::bind(&OfflineCameraComponent::onFovChange, this, std::placeholders::_1));

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			rprContextCreateCamera(feature->getContext(), &this->rprCamera_);

			rprCameraSetNearPlane(this->rprCamera_, this->nearPlane_);
			rprCameraSetFarPlane(this->rprCamera_, this->farPlane_);
			rprCameraSetFocalLength(this->rprCamera_, this->focalLength_);
			rprCameraSetFocusDistance(this->rprCamera_, this->focusDistance_);
			rprCameraSetFStop(this->rprCamera_, this->fStop_);
			rprCameraSetMode(this->rprCamera_, RPR_CAMERA_MODE_PERSPECTIVE);

			std::uint32_t w, h;
			feature->getFramebufferScale(w, h);
			rprCameraSetSensorSize(this->rprCamera_, filmSize_ *  w / (float)h, filmSize_);

			auto transform = this->getComponent<TransformComponent>();
			auto eye = transform->getTranslate();
			auto at = transform->getTranslate() + math::rotate(transform->getQuaternion(), math::float3::UnitZ);
			auto up = math::rotate(transform->getQuaternion(), math::float3::UnitY);
			rprCameraLookAt(this->rprCamera_, eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

			rprSceneSetCamera(feature->getScene(), this->rprCamera_);

			if (!path_.empty())
			{
				this->rprClearImage_ = this->createImage(path_);
				rprSceneSetBackgroundImage(feature->getScene(), this->rprClearImage_);
			}
			else
			{
				rpr_image_format imageFormat = { 3, RPR_COMPONENT_TYPE_FLOAT32 };
				rpr_image_desc imageDesc = { 1, 1, 1, 3, 3 };

				rprContextCreateImage(feature->getContext(), imageFormat, &imageDesc, clearColor_.data(), &this->rprClearImage_);
				rprSceneSetBackgroundImage(feature->getScene(), this->rprClearImage_);
			}

			feature->addOfflineListener(this);
		}
	}

	void
	OfflineCameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("Camera:fov", std::bind(&OfflineCameraComponent::onFovChange, this, std::placeholders::_1));

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			feature->removeOfflineListener(this);

			if (feature->getScene())
			{
				rprSceneSetCamera(feature->getScene(), nullptr);
				rprSceneSetBackgroundImage(feature->getScene(), nullptr);
			}
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
	OfflineCameraComponent::onPreRender() noexcept
	{
	}

	void
	OfflineCameraComponent::onPostRender() noexcept
	{
	}

	void
	OfflineCameraComponent::onLayerChangeAfter() noexcept
	{
	}

	void
	OfflineCameraComponent::onFrameDirty() noexcept
	{
		auto feature = this->tryGetFeature<OfflineFeature>();
		if (feature)
			feature->setFramebufferDirty(true);
	}

	void
	OfflineCameraComponent::onFovChange(const runtime::any& data) noexcept
	{
		if (data.type() == typeid(float))
			this->setAperture(runtime::any_cast<float>(data));
	}

	void*
	OfflineCameraComponent::createImage(const std::string& path) noexcept
	{
		image::Image image;
		if (!image.load(path))
			return nullptr;

		bool hasAlpha = false;
		std::uint8_t channel = 3;
		switch (image.format())
		{
		case image::Format::B8G8R8A8UNorm:
			hasAlpha = true;
			channel = 4;
			break;
		case image::Format::R8G8B8A8SNorm:
			hasAlpha = true;
			channel = 4;
			break;
		case image::Format::R8G8B8A8SRGB:
			hasAlpha = true;
			channel = 4;
			break;
		case image::Format::B8G8R8A8SRGB:
			hasAlpha = true;
			channel = 4;
			break;
		}

		rpr_image_format imageFormat;
		imageFormat.num_components = channel;
		imageFormat.type = image.format().value_type() == octoon::image::value_t::Float ? RPR_COMPONENT_TYPE_FLOAT32 : RPR_COMPONENT_TYPE_UINT8;

		rpr_image_desc imageDesc;
		imageDesc.image_width = image.width();
		imageDesc.image_height = image.height();
		imageDesc.image_depth = 1;
		imageDesc.image_row_pitch = imageDesc.image_width * imageFormat.num_components * (imageFormat.type == RPR_COMPONENT_TYPE_FLOAT32 ? 4 : 1);
		imageDesc.image_slice_pitch = imageDesc.image_row_pitch * imageDesc.image_height;

		auto feature = this->tryGetFeature<OfflineFeature>();
		if (feature)
		{
			rpr_image rprImage = nullptr;
			if (RPR_SUCCESS == rprContextCreateImage(feature->getContext(), imageFormat, &imageDesc, image.data(), &rprImage))
				return rprImage;
		}

		return nullptr;
	}
}