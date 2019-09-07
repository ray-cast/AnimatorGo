#include <octoon/offline_environment_light_component.h>
#include <octoon/offline_feature.h>
#include <octoon/transform_component.h>
#include <octoon/image/image.h>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubInterface(OfflineEnvironmentLightComponent, OfflineLightComponent, "OfflineEnvironmentLight")

	OfflineEnvironmentLightComponent::OfflineEnvironmentLightComponent() noexcept
		: rprLight_(nullptr)
		, rprImage_(nullptr)
		, color_(math::float3::One)
		, useBgImage_(true)
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

	void
	OfflineEnvironmentLightComponent::setBgImage(const std::string& path) noexcept
	{
		if (path_ != path)
		{
			if (rprLight_ && useBgImage_)
			{
				auto feature = this->tryGetFeature<OfflineFeature>();
				if (feature)
				{
					rpr_image rprImage = nullptr;

					if (path.empty())
					{
						rpr_image_format format = { 3, RPR_COMPONENT_TYPE_FLOAT32 };
						rpr_image_desc desc = { 1, 1, 1, 3, 3 };

						if (RPR_SUCCESS != rprContextCreateImage(feature->getContext(), format, &desc, color_.ptr(), &rprImage))
							return;
					}
					else
					{
						rprImage = this->createImage(path);
					}

					if (rprImage)
					{
						if (this->rprImage_)
						{
							rprObjectDelete(rprImage_);
							this->rprImage_ = nullptr;
						}

						this->rprImage_ = rprImage;

						if (RPR_SUCCESS != rprEnvironmentLightSetImage(this->rprLight_, rprImage))
							return;

						if (RPR_SUCCESS != rprSceneSetEnvironmentOverride(feature->getScene(), RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, rprImage))
							return;
					}

					feature->setFramebufferDirty(true);
				}
			}

			path_ = path;
		}
	}
	
	const std::string&
	OfflineEnvironmentLightComponent::getBgImage() const noexcept
	{
		return path_;
	}

	void
	OfflineEnvironmentLightComponent::setUseBgImage(bool enable) noexcept
	{
		if (useBgImage_ != enable)
		{
			if (rprLight_ && !path_.empty())
			{
				if (this->rprImage_)
				{
					rprObjectDelete(rprImage_);
					this->rprImage_ = nullptr;
				}

				auto feature = this->tryGetFeature<OfflineFeature>();
				if (feature)
				{
					if (enable)
					{
						this->rprImage_ = this->createImage(path_);
						rprEnvironmentLightSetImage(this->rprLight_, this->rprImage_);
					}
					else
					{
						rprEnvironmentLightSetImage(this->rprLight_, nullptr);
					}

					if (this->rprImage_)
						rprSceneSetEnvironmentOverride(feature->getScene(), RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, this->rprImage_);

					feature->setFramebufferDirty(true);
				}
			}

			useBgImage_ = enable;
		}
	}

	bool
	OfflineEnvironmentLightComponent::getUseBgImage() const noexcept
	{
		return useBgImage_;
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

			if (RPR_SUCCESS != rprContextCreateEnvironmentLight(feature->getContext(), &this->rprLight_))
				return;
			if (RPR_SUCCESS != rprEnvironmentLightSetIntensityScale(this->rprLight_, this->getIntensity()))
				return;
			if (RPR_SUCCESS != rprSceneAttachLight(feature->getScene(), this->rprLight_))
				return;
			if (RPR_SUCCESS != rprSceneSetBackgroundImage(feature->getScene(), nullptr))
				return;

			if (this->useBgImage_ && !path_.empty())
			{
				this->rprImage_ = this->createImage(path_);
				if (RPR_SUCCESS != rprEnvironmentLightSetImage(this->rprLight_, this->rprImage_))
					return;
			}
			else
			{
				if (RPR_SUCCESS != rprContextCreateImage(feature->getContext(), format, &desc, color_.ptr(), &this->rprImage_))
					return;
				if (RPR_SUCCESS != rprEnvironmentLightSetImage(this->rprLight_, this->rprImage_))
					return;
			}

			if (this->rprImage_)
				rprSceneSetEnvironmentOverride(feature->getScene(), RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, this->rprImage_);

			feature->setFramebufferDirty(true);
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

	void*
	OfflineEnvironmentLightComponent::createImage(const std::string& path) noexcept
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