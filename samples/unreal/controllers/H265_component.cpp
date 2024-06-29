#include "h265_component.h"
#include "unreal_behaviour.h"

extern "C"
{
#	include <libavformat/avformat.h>  
};

#include <x264.h>
#include <x265.h>
#include <fstream>
#include <filesystem>

namespace unreal
{
	H265Component::H265Component() noexcept
		: encoder_(nullptr)
		, picture_(nullptr)
		, param_(nullptr)
	{
	}

	void
	H265Component::onEnable() noexcept
	{
	}

	void
	H265Component::onDisable() noexcept
	{
		this->close();
	}

	bool
	H265Component::create(const std::filesystem::path& filepath) noexcept(false)
	{
		auto& context = this->getContext();
		auto& framebufferSize = this->getContext()->profile->cameraModule->framebufferSize.getValue();
		this->width_ = framebufferSize.x;
		this->height_ = framebufferSize.y;
		this->buf_ = std::make_unique<std::uint8_t[]>(this->width_ * this->height_ * 3);
		this->filepath_ = filepath;
		this->ostream_ = std::make_shared<std::ofstream>(std::filesystem::path(this->filepath_).append(".h265"), std::ios_base::binary);
		if (!this->ostream_->good())
			throw std::runtime_error("ofstream() failed");

		param_ = x265_param_alloc();
		if (!param_)
			throw std::runtime_error("x265_param_alloc() failed");

		x265_param_default(param_);
		x265_param_parse(param_, "--crf", std::to_string(this->getModel()->crf).c_str());
		x265_param_parse(param_, "--preset", "verslow");
		x265_param_parse(param_, "--qpmax", "59");
		x265_param_parse(param_, "--qcomp", "0.7");
		x265_param_parse(param_, "--aq-strength", "0.85");

		param_->bRepeatHeaders = 1;
		param_->internalCsp = X265_CSP_I420;
		param_->sourceWidth = this->width_;
		param_->sourceHeight = this->height_;
		param_->fpsNum = context->profile->playerModule->recordFps;
		param_->fpsDenom = 1;
		param_->bframes = 12;

		encoder_ = x265_encoder_open(param_);
		if (!encoder_)
			throw std::runtime_error("x265_encoder_open() failed");

		picture_ = x265_picture_alloc();
		if (!picture_)
			throw std::runtime_error("x265_picture_alloc() failed");

		x265_picture_init(param_, picture_);
		picture_->planes[0] = this->buf_.get();
		picture_->planes[1] = this->buf_.get() + param_->sourceWidth * param_->sourceHeight;
		picture_->planes[2] = this->buf_.get() + param_->sourceWidth * param_->sourceHeight * 5 / 4;
		picture_->stride[0] = param_->sourceWidth;
		picture_->stride[1] = param_->sourceWidth / 2;
		picture_->stride[2] = param_->sourceWidth / 2;
		picture_->height = param_->sourceHeight;

		return this->ostream_->good();
	}

	void
	H265Component::write(const octoon::math::Vector3* data) noexcept(false)
	{
		if (ostream_)
		{
			this->convert((float*)data, this->width_, this->height_, this->buf_.get());

			x265_nal* nals = nullptr;
			std::uint32_t inal = 0;
			auto result = x265_encoder_encode(encoder_, &nals, &inal, picture_, nullptr);
			if (result < 0)
				throw std::runtime_error("x265_encoder_encode() failed");

			for (std::uint32_t j = 0;j < inal; j++)
				ostream_->write((char*)nals[j].payload, nals[j].sizeBytes);
		}
	}

	void
	H265Component::close() noexcept
	{
		if (this->ostream_)
		{
			x265_nal* nals = nullptr;
			std::uint32_t inal = 0;

			while (x265_encoder_encode(encoder_, &nals, &inal, nullptr, nullptr) > 0)
			{
				for (std::uint32_t j = 0; j < inal; j++)
					ostream_->write((char*)nals[j].payload, nals[j].sizeBytes);
			}

			x265_encoder_close(encoder_);
			x265_picture_free(picture_);
			x265_param_free(param_);

			this->ostream_.reset();

			AVFormatContext* videoFormat = nullptr;
			AVFormatContext* audioFormat = nullptr;
			AVFormatContext* outputFormat = nullptr;

			AVPacket* packet = av_packet_alloc();

			auto outFilename = filepath_.string();
			auto inFilename = filepath_.string() + ".h265";

			try
			{
				auto& playerModule = this->getContext()->profile->playerModule;

				if (avformat_open_input(&videoFormat, inFilename.c_str(), 0, 0) < 0)
					throw std::runtime_error("Could not open input file.");

				if (avformat_find_stream_info(videoFormat, 0) < 0)
					throw std::runtime_error("Failed to retrieve input stream information");

				if (avformat_alloc_output_context2(&outputFormat, NULL, NULL, outFilename.c_str()) < 0)
				{
					printf("Could not deduce output format from file extension: using MPEG.\n");

					if (avformat_alloc_output_context2(&outputFormat, NULL, "mpeg", outFilename.c_str()) < 0)
						throw std::runtime_error("Could not create output context\n");
				}

				AVStream* video_in_stream = NULL;
				AVStream* video_out_stream = NULL;

				int video_stream_index = -1;
				for (unsigned int i = 0; i < videoFormat->nb_streams; i++)
				{
					if (videoFormat->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						videoFormat->streams[i]->codecpar->color_range = AVColorRange::AVCOL_RANGE_JPEG;

						auto istream = video_in_stream = videoFormat->streams[i];
						auto ostream = video_out_stream = avformat_new_stream(outputFormat, avcodec_find_encoder(istream->codecpar->codec_id));
						if (ostream == nullptr)
							throw std::runtime_error("Failed allocating output stream\n");

						if (avcodec_parameters_copy(ostream->codecpar, istream->codecpar) < 0)
							throw std::runtime_error("Failed to copy context from input to output stream codec context\n");

						video_stream_index = i;
						break;
					}
				}

				if (avio_open(&outputFormat->pb, outFilename.c_str(), AVIO_FLAG_WRITE) < 0)
					throw std::runtime_error("Could not open output file " + outFilename);

				if (avformat_write_header(outputFormat, NULL) < 0)
					throw std::runtime_error("Error occurred when opening output file\n");

				std::size_t frame_index = 0;

				while (av_read_frame(videoFormat, packet) >= 0)
				{
					if (packet->stream_index == video_stream_index)
					{
						auto istream = video_in_stream;
						auto ostream = video_out_stream;

						if (packet->pts == AV_NOPTS_VALUE)
						{
							auto calc_duration = (double)AV_TIME_BASE / av_q2d(av_make_q(playerModule->recordFps, 1));
							packet->pts = (double)(frame_index * calc_duration) / (double)(av_q2d(istream->time_base) * AV_TIME_BASE);
							packet->dts = packet->pts;
							packet->duration = (double)calc_duration / (double)(av_q2d(istream->time_base) * AV_TIME_BASE);
						}

						packet->pts = av_rescale_q_rnd(packet->pts, istream->time_base, ostream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
						packet->dts = av_rescale_q_rnd(packet->dts, istream->time_base, ostream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
						packet->duration = av_rescale_q(packet->duration, istream->time_base, ostream->time_base);
						packet->pos = -1;
						packet->stream_index = video_stream_index;

						frame_index++;

						if (av_interleaved_write_frame(outputFormat, packet) < 0)
							throw std::runtime_error("Error muxing packet");
					}

					av_packet_unref(packet);
				}

				if (av_write_trailer(outputFormat) < 0)
					throw std::runtime_error("Failed to write the trailer");

				if (packet)
					av_packet_free(&packet);

				if (videoFormat)
					avformat_close_input(&videoFormat);

				if (audioFormat)
					avformat_close_input(&audioFormat);

				avio_close(outputFormat->pb);
				avformat_free_context(outputFormat);

				std::filesystem::remove(inFilename);
			}
			catch (...)
			{
				if (packet)
					av_packet_free(&packet);

				if (videoFormat)
					avformat_close_input(&videoFormat);

				if (audioFormat)
					avformat_close_input(&audioFormat);

				if (outputFormat)
				{
					avio_close(outputFormat->pb);
					avformat_free_context(outputFormat);
				}

				if (std::filesystem::exists(outFilename))
					std::filesystem::remove(outFilename);
			}
		}
	}

	void
	H265Component::convert(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept
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