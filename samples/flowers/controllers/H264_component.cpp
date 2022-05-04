#include "h264_component.h"
#include "flower_behaviour.h"

extern "C"
{
#	include <libavformat/avformat.h>  
};

#include <x264.h>
#include <fstream>
#include <filesystem>

namespace flower
{
	H264Component::H264Component() noexcept
		: encoder_(nullptr)
		, encoded_frame_(nullptr)
		, frame_(nullptr)
	{
	}

	void
	H264Component::setActive(bool active) noexcept
	{
		if (this->getModel()->enable != active)
		{
			this->getModel()->enable = active;

			if (active)
				this->onEnable();
			else
				this->onDisable();
		}
	}

	bool
	H264Component::getActive() const noexcept
	{
		return this->getModel()->enable;
	}
	void
	H264Component::onEnable() noexcept
	{
	}

	void
	H264Component::onDisable() noexcept
	{
		if (this->ostream_)
		{
			int iFrameSize = 0;
			int iNal = 0;
			x264_nal_t* pNals = NULL;

			int frame_size = x264_encoder_encode(encoder_, &pNals, &iNal, frame_, encoded_frame_);
			if (frame_size > 0 && iNal > 0)
			{
				for (int i = 0; i < iNal; ++i)
				{
					std::int32_t i_num_nal_h = 0;

					if (pNals[i].i_payload > 4)
					{
						while (i_num_nal_h < 5 && pNals[i].p_payload[i_num_nal_h] == 0)
							i_num_nal_h++;

						static char nal_head_s[3] = { 0, 0, 0 };

						if (i_num_nal_h < 3)
						{
							ostream_->write(nal_head_s, 3 - i_num_nal_h);
							iFrameSize += (3 - i_num_nal_h);
						}
					}

					ostream_->write((char*)pNals[i].p_payload, pNals[i].i_payload);

					iFrameSize += pNals[i].i_payload;
				}
			}

			x264_encoder_close(encoder_);
			x264_picture_clean(frame_);
			x264_picture_clean(encoded_frame_);

			this->ostream_.reset();

			AVFormatContext* iformat = nullptr;
			AVFormatContext* oformat = nullptr;

			try
			{
				auto& playerModule = this->getContext()->profile->playerModule;

				auto inFilename = filepath_ + ".tmp";
				auto outFilename = filepath_;

				if (avformat_open_input(&iformat, inFilename.c_str(), 0, 0) < 0)
					throw std::runtime_error("Could not open input file.");

				if (avformat_find_stream_info(iformat, 0) < 0)
					throw std::runtime_error("Failed to retrieve input stream information");

				if (avformat_alloc_output_context2(&oformat, NULL, NULL, outFilename.c_str()) < 0)
					throw std::runtime_error("Could not create output context\n");

				int videoindex_v = 0, videoindex_out = 0;
				for (unsigned int i = 0; i < iformat->nb_streams; i++)
				{
					if (iformat->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						auto istream = iformat->streams[i];
						istream->codecpar->format = AV_PIX_FMT_YUVJ420P;
						istream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
						// istream->codecpar->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

						auto ostream = avformat_new_stream(oformat, avcodec_find_encoder(istream->codecpar->codec_id));
						if (!ostream)
							throw std::runtime_error("Failed allocating output stream\n");

						if (avcodec_parameters_copy(ostream->codecpar, istream->codecpar) < 0)
							throw std::runtime_error("Failed to copy context from input to output stream codec context\n");

						videoindex_v = i;
						videoindex_out = ostream->index;
						break;
					}
				}

				if (avio_open(&oformat->pb, outFilename.c_str(), AVIO_FLAG_WRITE) < 0)
					throw std::runtime_error("Could not open output file " + outFilename);

				if (avformat_write_header(oformat, NULL) < 0)
					throw std::runtime_error("Error occurred when opening output file\n");

				AVPacket packet;
				std::size_t frame_index = 0;

				while (av_read_frame(iformat, &packet) >= 0)
				{
					if (packet.stream_index == videoindex_v)
					{
						auto istream = iformat->streams[packet.stream_index];
						auto ostream = oformat->streams[videoindex_out];

						if (packet.pts == AV_NOPTS_VALUE)
						{
							auto calc_duration = (double)AV_TIME_BASE / av_q2d(av_make_q(playerModule->recordFps, 1));
							packet.pts = (double)(frame_index * calc_duration) / (double)(av_q2d(istream->time_base) * AV_TIME_BASE);
							packet.dts = packet.pts;
							packet.duration = (double)calc_duration / (double)(av_q2d(istream->time_base) * AV_TIME_BASE);
						}

						packet.pts = av_rescale_q_rnd(packet.pts, istream->time_base, ostream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
						packet.dts = av_rescale_q_rnd(packet.dts, istream->time_base, ostream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
						packet.duration = av_rescale_q(packet.duration, istream->time_base, ostream->time_base);
						packet.pos = -1;
						packet.stream_index = videoindex_out;

						frame_index++;

						if (av_interleaved_write_frame(oformat, &packet) < 0)
							throw std::runtime_error("Error muxing packet");
					}

					av_packet_unref(&packet);
				}

				if (av_write_trailer(oformat) < 0)
					throw std::runtime_error("Failed to write the trailer");

				avformat_close_input(&iformat);
				avio_close(oformat->pb);
				avformat_free_context(oformat);

				std::filesystem::remove(inFilename);
			}
			catch (...)
			{
				if (iformat)
					avformat_close_input(&iformat);

				if (oformat)
				{
					if (!(oformat->oformat->flags & AVFMT_NOFILE))
					{
						av_write_trailer(oformat);
						avio_close(oformat->pb);
					}

					avformat_free_context(oformat);
				}
			}
		}
	}

	bool
	H264Component::record(std::string_view filepath) noexcept(false)
	{
		auto& context = this->getContext();
		this->width_ = context->profile->recordModule->width;
		this->height_ = context->profile->recordModule->height;
		this->buf_ = std::make_unique<std::uint8_t[]>(this->width_ * this->height_ * 3);
		this->filepath_ = filepath;
		this->ostream_ = std::make_shared<std::ofstream>(this->filepath_ + ".tmp", std::ios_base::binary);
		if (!this->ostream_->good())
			throw std::runtime_error("ofstream() failed");

		x264_param_t encode_param_;
		x264_param_default(&encode_param_);
		x264_param_default_preset(&encode_param_, "fast", "zerolatency");
		x264_param_apply_profile(&encode_param_, "baseline");

		encode_param_.b_repeat_headers = 1;
		encode_param_.b_cabac = 1;
		encode_param_.b_sliced_threads = 0;

		encode_param_.i_frame_reference = 6;
		encode_param_.i_bframe = 6;
		encode_param_.i_keyint_min = 1;
		encode_param_.i_keyint_max = std::numeric_limits<int>::infinity();
		encode_param_.i_scenecut_threshold = 60;
		encode_param_.i_deblocking_filter_alphac0 = 1;
		encode_param_.i_deblocking_filter_beta = 1;
		encode_param_.i_level_idc = 40;
		encode_param_.i_log_level = X264_LOG_NONE;
		encode_param_.i_width = this->width_;
		encode_param_.i_height = this->height_;
		encode_param_.i_threads = 1;
		encode_param_.i_fps_num = context->profile->playerModule->recordFps;
		encode_param_.i_fps_den = 1;
		encode_param_.analyse.b_psnr = 1;
		encode_param_.analyse.f_psy_rd = 0.3;

		encode_param_.rc.i_rc_method = X264_RC_CRF;
		encode_param_.rc.i_aq_mode = X264_AQ_AUTOVARIANCE;
		encode_param_.rc.f_aq_strength = 0.8;
		encode_param_.rc.f_qcompress = 0.5f;
		encode_param_.rc.f_rf_constant = 10;
		encode_param_.rc.f_rf_constant_max = this->getModel()->crf;
		encode_param_.rc.f_rate_tolerance = 1.f;

		if ((encoder_ = x264_encoder_open(&encode_param_)) == NULL)
			return false;

		encoded_frame_ = new x264_picture_t;
		frame_ = new x264_picture_t;

		x264_picture_init(encoded_frame_);
		x264_picture_alloc(frame_, X264_CSP_I420, this->width_, this->height_);

		return this->ostream_->good();
	}

	void
	H264Component::write(const octoon::math::Vector3* data) noexcept(false)
	{
		if (ostream_)
		{
			auto& context = this->getContext();

			this->convert((float*)data, this->width_, this->height_, this->buf_.get());

			std::uint8_t* yuv[3];
			yuv[0] = this->buf_.get();
			yuv[1] = this->buf_.get() + this->width_ * this->height_;
			yuv[2] = this->buf_.get() + this->width_ * this->height_ * 5 / 4;

			int y_size = this->width_ * this->height_;
			memcpy(frame_->img.plane[0] + 0, yuv[0], y_size);
			memcpy(frame_->img.plane[0] + y_size, yuv[1], y_size / 4);
			memcpy(frame_->img.plane[0] + y_size * 5 / 4, yuv[2], y_size / 4);

			int iFrameSize = 0;
			int iNal = 0;
			x264_nal_t* pNals = NULL;

			int frame_size = x264_encoder_encode(encoder_, &pNals, &iNal, frame_, encoded_frame_);
			if (frame_size > 0 && iNal > 0)
			{
				for (int i = 0; i < iNal; ++i)
				{
					std::int32_t i_num_nal_h = 0;

					if (pNals[i].i_payload > 4)
					{
						while (i_num_nal_h < 5 && pNals[i].p_payload[i_num_nal_h] == 0)
							i_num_nal_h++;

						static char nal_head_s[3] = { 0, 0, 0 };

						if (i_num_nal_h < 3)
						{
							ostream_->write(nal_head_s, 3 - i_num_nal_h);
							iFrameSize += (3 - i_num_nal_h);
						}
					}

					ostream_->write((char*)pNals[i].p_payload, pNals[i].i_payload);

					iFrameSize += pNals[i].i_payload;
				}
			}
		}
	}

	void
	H264Component::convert(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept
	{
		auto ptrY = yuvBuf;
		auto ptrU = ptrY + this->width_ * this->height_;
		auto ptrV = ptrU + this->width_ * this->height_ / 4;

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