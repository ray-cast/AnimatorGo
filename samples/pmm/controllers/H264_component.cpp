#include "h264_component.h"
#include "mysticlit_behaviour.h"
#include <octoon/offline_camera_component.h>
#include <octoon/timer_feature.h>
#include <octoon/offline_feature.h>

#include <fstream>
#define MINIH264_IMPLEMENTATION
#include <minih264e.h>
#include <OpenImageDenoise/oidn.hpp>

namespace MysticLit
{
	H264Component::H264Component() noexcept
		: time_(0.0f)
		, timeStep_(0.0f)
		, device_(nullptr)
		, filter_(nullptr)
		, oidnColorBuffer_(nullptr)
		, oidnAlbedoBuffer_(nullptr)
		, oidnNormalBuffer_(nullptr)
		, oidnOutputBuffer_(nullptr)
		, active_(false)
	{
	}

	void
	H264Component::setActive(bool active) noexcept
	{
		active_ = active;
	}

	bool
	H264Component::getActive() const noexcept
	{
		return active_;
	}

	void
	H264Component::setTimeStep(float timeStep) noexcept
	{
		timeStep_ = timeStep;
	}

	float
	H264Component::getTimeStep() const noexcept
	{
		return timeStep_;
	}

	void
	H264Component::onEnable() noexcept
	{
	}

	void
	H264Component::onDisable() noexcept
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
	H264Component::play() noexcept
	{
		auto filepath = "C:/Users/ray/Desktop/1.264";
		this->capture(std::make_shared<std::ofstream>(filepath, octoon::io::ios_base::binary));
		active_ = true;
	}

	void
	H264Component::capture(const std::shared_ptr<std::ofstream>& stream) noexcept
	{
		auto camera = this->getContext()->profile->entitiesModule_->camera;
		if (camera)
		{
			auto framebuffer = camera->getComponent<octoon::OfflineCameraComponent>()->getFramebuffer();

			H264E_create_param_t create_param;
			std::memset(&create_param, 0, sizeof(create_param));
			create_param.enableNEON = 1;
			create_param.num_layers = 1;
			create_param.inter_layer_pred_flag = 1;
			create_param.inter_layer_pred_flag = 0;
			create_param.gop = 1;
			create_param.width = framebuffer->getGraphicsFramebufferDesc().getWidth();
			create_param.height = framebuffer->getGraphicsFramebufferDesc().getHeight();
			create_param.max_long_term_reference_frames = 0;
			create_param.fine_rate_control_flag = 0;
			create_param.const_input_flag = 0;
			create_param.vbv_size_bytes = 100000 / 8;
			create_param.temporal_denoise_flag = false;
			create_param.max_threads = 0;

			int sizeof_persist = 0, sizeof_scratch = 0;
			auto error = H264E_sizeof(&create_param, &sizeof_persist, &sizeof_scratch);

			this->enc_ = std::make_unique<char[]>(sizeof_persist);
			this->scratch_ = std::make_unique<char[]>(sizeof_scratch);
			this->buf_ = std::make_unique<std::uint8_t[]>(create_param.width * create_param.height * 3);

			error = H264E_init((H264E_persist_t*)this->enc_.get(), &create_param);

			this->width_ = create_param.width;
			this->height_ = create_param.height;
			this->ostream_ = stream;

			device_ = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
			oidnCommitDevice(device_);

			filter_ = oidnNewFilter(device_, "RT");

			oidnColorBuffer_ = oidnNewBuffer(device_, this->width_ * this->height_ * sizeof(octoon::math::float3));
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
		}
	}

	void
	H264Component::update() noexcept
	{
		if (timeStep_ > 0.0f)
		{
			auto feature = this->getContext()->behaviour->getFeature<octoon::TimerFeature>();
			if (feature)
			{
				time_ += feature->delta();

				if (time_ >= timeStep_)
				{
					this->capture();
					time_ -= timeStep_;
				}
			}
		}
		else
		{
			this->capture();
		}
	}

	void
	H264Component::capture() noexcept
	{
		H264E_io_yuv_t yuv;
		yuv.yuv[0] = (std::uint8_t*)this->buf_.get();
		yuv.yuv[1] = (std::uint8_t*)this->buf_.get() + this->width_ * this->height_;
		yuv.yuv[2] = (std::uint8_t*)this->buf_.get() + this->width_ * this->height_ * 5 / 4;
		yuv.stride[0] = this->width_;
		yuv.stride[1] = this->width_ / 2;
		yuv.stride[2] = this->width_ / 2;

		auto& model = this->getModel();

		H264E_run_param_t run_param;
		std::memset(&run_param, 0, sizeof(run_param));
		run_param.qp_min = model->qp_min;
		run_param.qp_max = model->qp_min;
		run_param.frame_type = model->frame_type;
		run_param.encode_speed = model->encode_speed;

		auto camera = this->getContext()->profile->entitiesModule_->camera;
		if (camera)
		{
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

			oidnUnmapBuffer(oidnOutputBuffer_, output);
		}

		int sizeof_coded_data;
		std::uint8_t* coded_data;
		if (H264E_STATUS_SUCCESS == H264E_encode((H264E_persist_t*)this->enc_.get(), (H264E_scratch_t*)this->scratch_.get(), &run_param, &yuv, &coded_data, &sizeof_coded_data))
			ostream_->write((char*)coded_data, sizeof_coded_data);
	}

	void
	H264Component::rgb2yuv(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept
	{
		auto ptrY = yuvBuf;
		auto ptrU = ptrY + w * h;
		auto ptrV = ptrU + w * h / 4;

		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				auto data = rgb + ((h - j - 1) * w + i) * 3;
				std::uint32_t r = data[0] * 255.0f;
				std::uint32_t g = data[1] * 255.0f;
				std::uint32_t b = data[2] * 255.0f;

				auto y = (std::uint8_t)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
				auto u = (std::uint8_t)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
				auto v = (std::uint8_t)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

				*(ptrY++) = std::clamp<std::uint8_t>(y, 0, 255);

				if (j % 2 == 0 && i % 2 == 0)
				{
					*(ptrU++) = std::clamp<std::uint8_t>(u, 0, 255);
				}
				else if (i % 2 == 0)
				{
					*(ptrV++) = std::clamp<std::uint8_t>(v, 0, 255);
				}
			}
		}
	}

	void
	H264Component::rgb2yuv(std::uint8_t* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept
	{
		auto ptrY = yuvBuf;
		auto ptrU = ptrY + w * h;
		auto ptrV = ptrU + w * h / 4;

		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				auto data = rgb + ((h - j - 1) * w + i) * 4;
				auto r = data[0];
				auto g = data[1];
				auto b = data[2];

				auto y = (std::uint8_t)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
				auto u = (std::uint8_t)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
				auto v = (std::uint8_t)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

				*(ptrY++) = std::clamp<std::uint8_t>(y, 0, 255);

				if (j % 2 == 0 && i % 2 == 0)
				{
					*(ptrU++) = std::clamp<std::uint8_t>(u, 0, 255);
				}
				else if (i % 2 == 0)
				{
					*(ptrV++) = std::clamp<std::uint8_t>(v, 0, 255);
				}
			}
		}
	}
}