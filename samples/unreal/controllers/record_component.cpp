#include "record_component.h"
#include "unreal_behaviour.h"
#include <OpenImageDenoise/oidn.hpp>
#include <octoon/camera_component.h>
#include <octoon/texture/texture.h>
#include <octoon/video_feature.h>

namespace unreal
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
	RecordComponent::onInit() noexcept
	{
		this->addMessageListener("editor:player:record", std::bind(&RecordComponent::onRecord, this));

		this->getContext()->profile->playerModule->finish += [this](bool value)
		{
			this->stopRecord();
		};
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
	RecordComponent::setupDenoise() noexcept
	{
		auto& profile = this->getContext()->profile;
		auto& framebufferSize = profile->cameraModule->framebufferSize.getValue();

		if (profile->offlineModule->getEnable() && profile->recordModule->denoise)
		{
			device_ = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
			oidnCommitDevice(device_);

			oidnColorBuffer_ = oidnNewBuffer(device_, framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
			oidnNormalBuffer_ = oidnNewBuffer(device_, framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
			oidnAlbedoBuffer_ = oidnNewBuffer(device_, framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
			oidnOutputBuffer_ = oidnNewBuffer(device_, framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));

			filter_ = oidnNewFilter(device_, "RT");
			oidnSetFilterImage(filter_, "color", oidnColorBuffer_, OIDN_FORMAT_FLOAT3, framebufferSize.x, framebufferSize.y, 0, 0, 0);
			oidnSetFilterImage(filter_, "albedo", oidnAlbedoBuffer_, OIDN_FORMAT_FLOAT3, framebufferSize.x, framebufferSize.y, 0, 0, 0);
			oidnSetFilterImage(filter_, "normal", oidnNormalBuffer_, OIDN_FORMAT_FLOAT3, framebufferSize.x, framebufferSize.y, 0, 0, 0);
			oidnSetFilterImage(filter_, "output", oidnOutputBuffer_, OIDN_FORMAT_FLOAT3, framebufferSize.x, framebufferSize.y, 0, 0, 0);
			oidnSetFilter1b(filter_, "hdr", this->getModel()->hdr);
			oidnSetFilter1b(filter_, "srgb", this->getModel()->srgb);
			oidnCommitFilter(filter_);
		}
	}

	void
	RecordComponent::shutdownDenoise() noexcept
	{
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
	RecordComponent::captureImage() noexcept
	{
		auto& context = this->getContext();
		auto& profile = this->getContext()->profile;
		auto& framebufferSize = profile->cameraModule->framebufferSize.getValue();

		if (profile->offlineModule->getEnable())
		{
			auto videoFeature = this->getFeature<octoon::VideoFeature>();
			videoFeature->readColorBuffer(colorBuffer_.data());
			videoFeature->readNormalBuffer(normalBuffer_.data());
			videoFeature->readAlbedoBuffer(albedoBuffer_.data());
		}
		else
		{
			auto camera = context->profile->cameraModule->camera.getValue()->getComponent<octoon::CameraComponent>();
			auto colorTexture = camera->getFramebuffer()->getFramebufferDesc().getColorAttachments().front().getBindingTexture();
			if (colorTexture)
			{
				auto& desc = colorTexture->getTextureDesc();

				void* data = nullptr;
				if (colorTexture->map(0, 0, desc.getWidth(), desc.getHeight(), 0, &data))
				{
					if (desc.getTexFormat() == octoon::GraphicsFormat::R32G32B32A32SFloat)
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

		if (profile->offlineModule->getEnable() && profile->recordModule->denoise)
		{
			auto color = oidnMapBuffer(oidnColorBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto normal = oidnMapBuffer(oidnNormalBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto albedo = oidnMapBuffer(oidnAlbedoBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);

			std::memcpy(color, colorBuffer_.data(), framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
			std::memcpy(normal, normalBuffer_.data(), framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
			std::memcpy(albedo, albedoBuffer_.data(), framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));

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
				std::memcpy(denoiseBuffer_.data(), output, framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
				oidnUnmapBuffer(oidnOutputBuffer_, output);
			}
		}
		else
		{
			std::memcpy(denoiseBuffer_.data(), colorBuffer_.data(), framebufferSize.x * framebufferSize.y * sizeof(octoon::math::float3));
		}

		auto markComponent = this->getComponent<MarkComponent>();
		if (markComponent && markComponent->getActive())
		{
			auto markModel = markComponent->getModel();

			auto width = std::min<std::uint32_t>(markModel->width, framebufferSize.x);
			auto height = std::min<std::uint32_t>(markModel->height, framebufferSize.y);

			for (std::uint32_t y = 0; y < height; y++)
			{
				for (std::uint32_t x = 0; x < width; x++)
				{
					auto src = ((markModel->height - y - 1) * markModel->width + x) * markModel->channel;
					auto& dst = denoiseBuffer_[(y + markModel->y) * framebufferSize.x + x + markModel->x];

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
	RecordComponent::captureImage(const std::filesystem::path& filepath) noexcept
	{
		auto& framebufferSize = this->getContext()->profile->cameraModule->framebufferSize.getValue();

		this->albedoBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->normalBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->colorBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->denoiseBuffer_.resize(framebufferSize.x * framebufferSize.y);

		try
		{
			this->setupDenoise();
			this->captureImage();
			this->shutdownDenoise();

			auto width = framebufferSize.x;
			auto height = framebufferSize.y;
			auto output = this->getContext()->profile->offlineModule->getEnable() ? denoiseBuffer_.data() : colorBuffer_.data();

			octoon::Texture image;

			if (image.create(octoon::Format::R8G8B8SRGB, width, height))
			{
				auto data = (char*)image.data();

#pragma omp parallel for num_threads(4)
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

				image.save(filepath, "png");
			}
		}
		catch (...)
		{
			this->shutdownDenoise();
		}
	}

	bool
	RecordComponent::startRecord(const std::filesystem::path& filepath) noexcept
	{
		auto& profile = this->getContext()->profile;
		auto& framebufferSize = this->getContext()->profile->cameraModule->framebufferSize.getValue();

		this->albedoBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->normalBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->colorBuffer_.resize(framebufferSize.x * framebufferSize.y);
		this->denoiseBuffer_.resize(framebufferSize.x * framebufferSize.y);

		if (profile->encodeModule->enable)
		{
			if (profile->encodeModule->encodeMode == EncodeMode::H264)
			{
				auto h264 = this->getComponent<H264Component>();
				if (!h264->create(filepath))
					return false;
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::H265)
			{
				auto h265 = this->getComponent<H265Component>();
				if (!h265->create(filepath))
					return false;
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::Frame)
			{
				auto frameEncoder = this->getComponent<FrameSequenceComponent>();
				if (!frameEncoder->create(filepath))
					return false;
			}
			else
			{
				return false;
			}
		}

		this->setupDenoise();

		profile->recordModule->active = true;

		return true;
	}

	void
	RecordComponent::stopRecord() noexcept(false)
	{
		auto& profile = this->getContext()->profile;

		profile->recordModule->active = false;

		if (profile->encodeModule->enable)
		{
			if (profile->encodeModule->encodeMode == EncodeMode::H264)
			{
				auto h264 = this->getComponent<H264Component>();
				if (h264)
					h264->close();
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::H265)
			{
				auto h265 = this->getComponent<H265Component>();
				if (h265)
					h265->close();
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::Frame)
			{
				auto frameEncoder = this->getComponent<FrameSequenceComponent>();
				if (frameEncoder)
					frameEncoder->close();
			}
			else
			{
				throw std::runtime_error("Unsupported encode mode.");
			}
		}

		this->shutdownDenoise();
	}

	void
	RecordComponent::onRecord() noexcept(false)
	{
		auto& profile = this->getContext()->profile;
		if (profile->encodeModule->enable && profile->recordModule->active)
		{
			this->captureImage();

			if (profile->encodeModule->encodeMode == EncodeMode::H264)
			{
				auto h264 = this->getComponent<H264Component>();
				h264->write(this->denoiseBuffer_.data());
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::H265)
			{
				auto h265 = this->getComponent<H265Component>();
				h265->write(this->denoiseBuffer_.data());
			}
			else if (profile->encodeModule->encodeMode == EncodeMode::Frame)
			{
				auto frameEncoder = this->getComponent<FrameSequenceComponent>();
				frameEncoder->write(this->denoiseBuffer_.data());
			}
			else
			{
				throw std::runtime_error("Unsupported encode mode.");
			}
		}
	}
}