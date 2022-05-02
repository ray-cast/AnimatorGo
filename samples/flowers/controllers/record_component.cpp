#include "record_component.h"
#include "flower_behaviour.h"
#include <octoon/camera_component.h>
#include <octoon/image/image.h>
#include <octoon/hal/graphics.h>
#include <octoon/video_feature.h>
#include <OpenImageDenoise/oidn.hpp>

namespace flower
{
	RecordComponent::RecordComponent() noexcept
		: device_(nullptr)
		, filter_(nullptr)
		, oidnColorBuffer_(nullptr)
		, oidnNormalBuffer_(nullptr)
		, oidnAlbedoBuffer_(nullptr)
		, oidnOutputBuffer_(nullptr)
	{
	}

	RecordComponent::~RecordComponent() noexcept
	{
	}

	void
	RecordComponent::onEnable() noexcept
	{
	}

	void
	RecordComponent::onDisable() noexcept
	{
		this->stopRecord();
	}

	void
	RecordComponent::onResize(const octoon::input::InputEvent& event) noexcept
	{
		if (event.change.w > 0 && event.change.h > 0)
			this->getModel()->resize(event.change.w, event.change.h);
	}

	void
	RecordComponent::captureImage() noexcept
	{
		auto& model = this->getModel();
		auto& context = this->getContext();

		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			auto videoFeature = this->getFeature<octoon::VideoFeature>();
			videoFeature->readColorBuffer(colorBuffer_.data());
			videoFeature->readNormalBuffer(normalBuffer_.data());
			videoFeature->readAlbedoBuffer(albedoBuffer_.data());
		}
		else
		{
			auto camera = context->profile->entitiesModule->camera->getComponent<octoon::CameraComponent>();
			auto colorTexture = camera->getFramebuffer()->getFramebufferDesc().getColorAttachments().front().getBindingTexture();
			if (colorTexture)
			{
				auto& desc = colorTexture->getTextureDesc();

				void* data = nullptr;
				if (colorTexture->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
				{
					if (desc.getTexFormat() == octoon::hal::GraphicsFormat::R32G32B32A32SFloat)
					{
						for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); ++i)
							colorBuffer_[i] = (((octoon::math::float4*)data) + i)->xyz();

						for (std::size_t i = 0; i < desc.getWidth() * desc.getHeight(); ++i)
							denoiseBuffer_[i] = (((octoon::math::float4*)data) + i)->xyz();
					}
					else
					{
						std::memcpy(colorBuffer_.data(), data, desc.getWidth() * desc.getHeight() * 3 * sizeof(float));
						std::memcpy(denoiseBuffer_.data(), data, desc.getWidth() * desc.getHeight() * 3 * sizeof(float));
					}

					colorTexture->unmap();
				}
			}
		}

		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			auto color = oidnMapBuffer(oidnColorBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto normal = oidnMapBuffer(oidnNormalBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto albedo = oidnMapBuffer(oidnAlbedoBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);

			std::memcpy(color, colorBuffer_.data(), model->width * model->height * sizeof(octoon::math::float3));
			std::memcpy(normal, normalBuffer_.data(), model->width * model->height * sizeof(octoon::math::float3));
			std::memcpy(albedo, albedoBuffer_.data(), model->width * model->height * sizeof(octoon::math::float3));

			oidnUnmapBuffer(oidnColorBuffer_, color);
			oidnUnmapBuffer(oidnNormalBuffer_, normal);
			oidnUnmapBuffer(oidnAlbedoBuffer_, albedo);

			oidnExecuteFilter(filter_);

			const char* errorMessage;
			if (oidnGetDeviceError(device_, &errorMessage) != OIDN_ERROR_NONE)
				std::cout << "Error: " << errorMessage << std::endl;

			auto output = oidnMapBuffer(oidnOutputBuffer_, OIDN_ACCESS_READ, 0, 0);
			if (output)
			{
				std::memcpy(denoiseBuffer_.data(), output, model->width * model->height * sizeof(octoon::math::float3));
				oidnUnmapBuffer(oidnOutputBuffer_, output);
			}
		}

		auto markComponent = this->getComponent<MarkComponent>();
		if (markComponent && markComponent->getActive())
		{
			auto markModel = markComponent->getModel();

			auto width = std::min(markModel->width, model->width);
			auto height = std::min(markModel->height, model->height);

			for (std::uint32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					auto src = ((markModel->height - y - 1) * markModel->width + x) * markModel->channel;
					auto& dst = denoiseBuffer_[(y + markModel->y) * model->width + x + markModel->x];

					auto b = markModel->pixels[src] / 255.0f;
					auto g = markModel->pixels[src + 1] / 255.0f;
					auto r = markModel->pixels[src + 2] / 255.0f;

					float alpha = 1.0f;
					if (markModel->channel == 4)
						alpha = markModel->pixels[src + 3] / 255.0f;

					dst = octoon::math::lerp(dst, octoon::math::float3(r, g, b), alpha);
				}
			}
		}
	}

	void
	RecordComponent::captureImage(std::string_view filepath) noexcept
	{
		auto& model = this->getModel();

		this->albedoBuffer_.resize(model->width * model->height);
		this->normalBuffer_.resize(model->width * model->height);
		this->colorBuffer_.resize(model->width * model->height);
		this->denoiseBuffer_.resize(model->width * model->height);

		this->captureImage();

		auto canvas = this->getContext()->profile->recordModule;
		auto width = canvas->width;
		auto height = canvas->height;
		auto output = this->getContext()->profile->offlineModule->offlineEnable ? denoiseBuffer_.data() : colorBuffer_.data();

		octoon::Image image;

		if (image.create(octoon::Format::R8G8B8SRGB, width, height))
		{
			auto data = (char*)image.data();

#			pragma omp parallel for num_threads(4)
			for (std::int32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					auto src = y * width + x;
					auto dst = ((height - y - 1) * width + x) * 3;

					data[dst] = (std::uint8_t)(output[src].x * 255);
					data[dst + 1] = (std::uint8_t)(output[src].y * 255);
					data[dst + 2] = (std::uint8_t)(output[src].z * 255);
				}
			}

			image.save(std::string(filepath), "png");
		}
	}

	bool
	RecordComponent::startRecord(std::string_view filepath) noexcept
	{
		auto& model = this->getContext()->profile->recordModule;
		auto& profile = this->getContext()->profile;

		this->albedoBuffer_.resize(model->width * model->height);
		this->normalBuffer_.resize(model->width * model->height);
		this->colorBuffer_.resize(model->width * model->height);
		this->denoiseBuffer_.resize(model->width * model->height);

		if (profile->h265Module->enable)
		{
			auto h265 = this->getComponent<H265Component>();
			if (!h265->create(filepath))
				return false;
		}

		if (profile->offlineModule->offlineEnable)
		{
			device_ = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
			oidnCommitDevice(device_);

			oidnColorBuffer_ = oidnNewBuffer(device_, model->width * model->height * sizeof(octoon::math::float3));
			oidnNormalBuffer_ = oidnNewBuffer(device_, model->width * model->height * sizeof(octoon::math::float3));
			oidnAlbedoBuffer_ = oidnNewBuffer(device_, model->width * model->height * sizeof(octoon::math::float3));
			oidnOutputBuffer_ = oidnNewBuffer(device_, model->width * model->height * sizeof(octoon::math::float3));

			filter_ = oidnNewFilter(device_, "RT");
			oidnSetFilterImage(filter_, "color", oidnColorBuffer_, OIDN_FORMAT_FLOAT3, model->width, model->height, 0, 0, 0);
			oidnSetFilterImage(filter_, "albedo", oidnAlbedoBuffer_, OIDN_FORMAT_FLOAT3, model->width, model->height, 0, 0, 0);
			oidnSetFilterImage(filter_, "normal", oidnNormalBuffer_, OIDN_FORMAT_FLOAT3, model->width, model->height, 0, 0, 0);
			oidnSetFilterImage(filter_, "output", oidnOutputBuffer_, OIDN_FORMAT_FLOAT3, model->width, model->height, 0, 0, 0);
			oidnSetFilter1b(filter_, "hdr", this->getModel()->hdr);
			oidnSetFilter1b(filter_, "srgb", this->getModel()->srgb);
			oidnCommitFilter(filter_);
		}

		profile->recordModule->active = true;

		this->addMessageListener("flower:player:record", std::bind(&RecordComponent::onRecord, this));

		return true;
	}

	void
	RecordComponent::stopRecord() noexcept
	{
		auto& model = this->getContext()->profile->recordModule;
		auto& profile = this->getContext()->profile;

		profile->recordModule->active = false;

		this->removeMessageListener("flower:player:record", std::bind(&RecordComponent::onRecord, this));

		auto h265 = this->getComponent<H265Component>();
		if (h265)
			h265->close();

		if (oidnColorBuffer_)
		{
			oidnReleaseBuffer(oidnColorBuffer_);
			oidnColorBuffer_ = nullptr;
		}

		if (oidnNormalBuffer_)
		{
			oidnReleaseBuffer(oidnNormalBuffer_);
			oidnNormalBuffer_ = nullptr;
		}

		if (oidnAlbedoBuffer_)
		{
			oidnReleaseBuffer(oidnAlbedoBuffer_);
			oidnAlbedoBuffer_ = nullptr;
		}

		if (filter_)
		{
			oidnReleaseFilter(filter_);
			filter_ = nullptr;
		}

		if (device_)
		{
			oidnReleaseDevice(device_);
			device_ = nullptr;
		}
	}

	void
	RecordComponent::onRecord() noexcept
	{
		auto& profile = this->getContext()->profile;

		if (profile->h265Module->enable)
		{
			this->captureImage();

			auto h265 = this->getComponent<H265Component>();
			h265->write(this->denoiseBuffer_.data());
		}
	}
}