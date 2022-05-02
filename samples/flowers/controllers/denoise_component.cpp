#include "denoise_component.h"
#include "rabbit_behaviour.h"
#include <OpenImageDenoise/oidn.hpp>

namespace rabbit
{
	DenoiseComponent::DenoiseComponent() noexcept
		: device_(nullptr)
		, filter_(nullptr)
		, oidnColorBuffer_(nullptr)
		, oidnAlbedoBuffer_(nullptr)
		, oidnNormalBuffer_(nullptr)
		, oidnOutputBuffer_(nullptr)
	{
	}

	DenoiseComponent::~DenoiseComponent() noexcept
	{
	}

	void
	DenoiseComponent::setActive(bool active) noexcept
	{
		this->getModel()->active_ = active;
	}

	bool
	DenoiseComponent::getActive() const noexcept
	{
		return this->getModel()->active_;
	}

	void
	DenoiseComponent::onEnable() noexcept
	{
		device_ = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
		oidnCommitDevice(device_);

		auto& window = this->getContext()->profile->canvasModule;
		oidnColorBuffer_ = oidnNewBuffer(device_, window->width * window->height * sizeof(octoon::math::float3));
		oidnNormalBuffer_ = oidnNewBuffer(device_, window->width * window->height * sizeof(octoon::math::float3));
		oidnAlbedoBuffer_ = oidnNewBuffer(device_, window->width * window->height * sizeof(octoon::math::float3));
		oidnOutputBuffer_ = oidnNewBuffer(device_, window->width * window->height * sizeof(octoon::math::float3));

		filter_ = oidnNewFilter(device_, "RT");
		oidnSetFilterImage(filter_, "color", oidnColorBuffer_, OIDN_FORMAT_FLOAT3, window->width, window->height, 0, 0, 0);
		oidnSetFilterImage(filter_, "albedo", oidnAlbedoBuffer_, OIDN_FORMAT_FLOAT3, window->width, window->height, 0, 0, 0);
		oidnSetFilterImage(filter_, "normal", oidnNormalBuffer_, OIDN_FORMAT_FLOAT3, window->width, window->height, 0, 0, 0);
		oidnSetFilterImage(filter_, "output", oidnOutputBuffer_, OIDN_FORMAT_FLOAT3, window->width, window->height, 0, 0, 0);
		oidnSetFilter1b(filter_, "hdr", this->getModel()->hdr);
		oidnSetFilter1b(filter_, "srgb", this->getModel()->srgb);
		oidnCommitFilter(filter_);
	}

	void
	DenoiseComponent::onDisable() noexcept
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
	DenoiseComponent::onPostProcess() noexcept
	{
		if (this->getContext()->profile->offlineModule->offlineEnable)
		{
			auto& window = this->getContext()->profile->canvasModule;

			auto color = oidnMapBuffer(oidnColorBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto normal = oidnMapBuffer(oidnNormalBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
			auto albedo = oidnMapBuffer(oidnAlbedoBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);

			std::memcpy(color, window->colorBuffer.data(), window->width * window->height * sizeof(octoon::math::float3));
			std::memcpy(normal, window->normalBuffer.data(), window->width * window->height * sizeof(octoon::math::float3));
			std::memcpy(albedo, window->albedoBuffer.data(), window->width * window->height * sizeof(octoon::math::float3));

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
				std::memcpy(window->outputBuffer.data(), output, window->width * window->height * sizeof(octoon::math::float3));
				oidnUnmapBuffer(oidnOutputBuffer_, output);
			}
		}
	}
}