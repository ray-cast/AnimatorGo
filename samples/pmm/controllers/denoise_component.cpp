#include "denoise_component.h"
#include "mysticlit_behaviour.h"
#include <OpenImageDenoise/oidn.hpp>

namespace MysticLit
{
	DenoiseComponent::DenoiseComponent() noexcept
		: device_(nullptr)
		, filter_(nullptr)
		, oidnColorBuffer_(nullptr)
		, oidnAlbedoBuffer_(nullptr)
		, oidnNormalBuffer_(nullptr)
		, oidnOutputBuffer_(nullptr)
		, active_(false)
	{
	}

	void
	DenoiseComponent::setActive(bool active) noexcept
	{
		active_ = active;
	}

	bool
	DenoiseComponent::getActive() const noexcept
	{
		return active_;
	}

	void
	DenoiseComponent::onEnable() noexcept
	{
		device_ = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
		oidnCommitDevice(device_);

		filter_ = oidnNewFilter(device_, "RT");

		/*oidnColorBuffer_ = oidnNewBuffer(device_, this->width_ * this->height_ * sizeof(octoon::math::float3));
		oidnNormalBuffer_ = oidnNewBuffer(device_, this->width_ * this->height_ * sizeof(octoon::math::float3));
		oidnAlbedoBuffer_ = oidnNewBuffer(device_, this->width_ * this->height_ * sizeof(octoon::math::float3));
		oidnOutputBuffer_ = oidnNewBuffer(device_, this->width_ * this->height_ * sizeof(octoon::math::float3));

		oidnSetFilterImage(filter_, "color", oidnColorBuffer_, OIDN_FORMAT_FLOAT3, this->width_, this->height_, 0, 0, 0);
		oidnSetFilterImage(filter_, "albedo", oidnNormalBuffer_, OIDN_FORMAT_FLOAT3, this->width_, this->height_, 0, 0, 0);
		oidnSetFilterImage(filter_, "normal", oidnAlbedoBuffer_, OIDN_FORMAT_FLOAT3, this->width_, this->height_, 0, 0, 0);
		oidnSetFilterImage(filter_, "output", oidnOutputBuffer_, OIDN_FORMAT_FLOAT3, this->width_, this->height_, 0, 0, 0);
		oidnSetFilter1b(filter_, "hdr", false);
		oidnSetFilter1b(filter_, "srgb", true);
		oidnCommitFilter(filter_);

		auto color = oidnMapBuffer(oidnColorBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
		auto normal = oidnMapBuffer(oidnNormalBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);
		auto albedo = oidnMapBuffer(oidnAlbedoBuffer_, OIDN_ACCESS_WRITE_DISCARD, 0, 0);

		auto offlineFeature = this->getContext()->behaviour->getFeature<octoon::OfflineFeature>();
		offlineFeature->readColorFramebuffer(color);
		offlineFeature->readAlbedoFramebuffer(normal);
		offlineFeature->readNormalFramebuffer(albedo);

		oidnUnmapBuffer(oidnColorBuffer_, color);
		oidnUnmapBuffer(oidnNormalBuffer_, normal);
		oidnUnmapBuffer(oidnAlbedoBuffer_, albedo);

		oidnExecuteFilter(filter_);

		const char* errorMessage;
		if (oidnGetDeviceError(device_, &errorMessage) != OIDN_ERROR_NONE)
			std::cout << "Error: " << errorMessage << std::endl;

		auto output = oidnMapBuffer(oidnOutputBuffer_, OIDN_ACCESS_READ, 0, 0);
		rgb2yuv((float*)output, this->width_, this->height_, this->buf_.get());

		oidnUnmapBuffer(oidnOutputBuffer_, output);*/
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
}