#include "h264_component.h"
#include "mysticlit_behaviour.h"

#define MINIH264_IMPLEMENTATION
#include <minih264e.h>
#include <fstream>

namespace MysticLit
{
	H264Component::H264Component() noexcept
		: active_(false)
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
	H264Component::onEnable() noexcept
	{
	}

	void
	H264Component::onDisable() noexcept
	{
	}

	void
	H264Component::record(const std::string& filepath) noexcept
	{
		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (camera)
		{
			auto& context = this->getContext();

			H264E_create_param_t create_param;
			std::memset(&create_param, 0, sizeof(create_param));
			create_param.enableNEON = 1;
			create_param.num_layers = 1;
			create_param.inter_layer_pred_flag = 1;
			create_param.inter_layer_pred_flag = 0;
			create_param.gop = 1;
			create_param.width = context->profile->canvasModule->width;
			create_param.height = context->profile->canvasModule->height;
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
			this->ostream_ = std::make_shared<std::ofstream>(filepath, std::ios_base::binary);
		}
	}

	void
	H264Component::onPostProcess() noexcept
	{
		H264E_io_yuv_t yuv;
		yuv.yuv[0] = (std::uint8_t*)this->buf_.get();
		yuv.yuv[1] = (std::uint8_t*)this->buf_.get() + this->width_ * this->height_;
		yuv.yuv[2] = (std::uint8_t*)this->buf_.get() + this->width_ * this->height_ * 5 / 4;
		yuv.stride[0] = this->width_;
		yuv.stride[1] = this->width_ / 2;
		yuv.stride[2] = this->width_ / 2;

		auto& model = this->getModel();
		auto& context = this->getContext();

		H264E_run_param_t run_param;
		std::memset(&run_param, 0, sizeof(run_param));
		run_param.qp_min = model->qp_min;
		run_param.qp_max = model->qp_max;
		run_param.desired_frame_bytes = model->kbps * 1000 / 8 / model->fps;
		run_param.frame_type = model->frame_type;
		run_param.encode_speed = model->encode_speed;

		auto output = context->profile->canvasModule->outputBuffer.data();
		rgb2yuv((float*)output, this->width_, this->height_, this->buf_.get());

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
				std::int32_t r = (std::int32_t)(data[0] * 255.0f);
				std::int32_t g = (std::int32_t)(data[1] * 255.0f);
				std::int32_t b = (std::int32_t)(data[2] * 255.0f);

				auto y = (std::uint8_t)std::clamp(((66 * r + 129 * g + 25 * b + 128) >> 8) + 16, 0, 255);
				auto u = (std::uint8_t)std::clamp(((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128, 0, 255);
				auto v = (std::uint8_t)std::clamp(((112 * r - 94 * g - 18 * b + 128) >> 8) + 128, 0, 255);

				*(ptrY++) = y;

				if (j % 2 == 0 && i % 2 == 0)
					*(ptrU++) = u;
				else if (i % 2 == 0)
					*(ptrV++) = v;
			}
		}
	}
}