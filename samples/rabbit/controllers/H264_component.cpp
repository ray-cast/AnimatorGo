#include "h264_component.h"
#include "rabbit_behaviour.h"

extern "C"
{
#	include <libavformat/avformat.h>  
};

#include <x264.h>
#include <x265.h>
#include <fstream>
#include <filesystem>

namespace rabbit
{
	H264Component::H264Component() noexcept
		: encoder_(nullptr)
		, picture_(nullptr)
		, param_(nullptr)
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
			x265_nal* nals = nullptr;
			std::uint32_t inal = 0;

			while (x265_encoder_encode(encoder_, &nals, &inal, nullptr, nullptr) > 0)
			{
				for (std::uint32_t j = 0;j < inal;j++)
					ostream_->write((char*)nals[j].payload, nals[j].sizeBytes);
			}

			x265_encoder_close(encoder_);
			x265_picture_free(picture_);
			x265_param_free(param_);

			this->ostream_.reset();

			AVFormatContext* iformat = nullptr;
			AVFormatContext* oformat = nullptr;

			try
			{
				auto& timeModule = this->getContext()->profile->timeModule;
			
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
					if (iformat->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
					{
						auto istream = iformat->streams[i];
						istream->codec->pix_fmt = AV_PIX_FMT_YUVJ420P;
						istream->codec->codec_type = AVMEDIA_TYPE_VIDEO;
						istream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

						auto ostream = avformat_new_stream(oformat, istream->codec->codec);
						if (!ostream)
							throw std::runtime_error("Failed allocating output stream\n");

						if (avcodec_copy_context(ostream->codec, istream->codec) < 0)
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
							auto calc_duration = (double)AV_TIME_BASE / av_q2d(av_make_q(timeModule->recordFps, 1));
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

					av_free_packet(&packet);
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
		auto camera = this->getContext()->profile->entitiesModule->camera;
		if (!camera)
			return false;
		
		auto& context = this->getContext();
		this->width_ = context->profile->canvasModule->width;
		this->height_ = context->profile->canvasModule->height;
		this->buf_ = std::make_unique<std::uint8_t[]>(this->width_ * this->height_ * 3);
		this->filepath_ = filepath;
		this->ostream_ = std::make_shared<std::ofstream>(this->filepath_ + ".tmp", std::ios_base::binary);
		if (!this->ostream_->good())
			throw std::runtime_error("ofstream() failed");

		param_ = x265_param_alloc();
		if (!param_)
			throw std::runtime_error("x265_param_alloc() failed");

		x265_param_default(param_);
		x265_param_parse(param_, "--crf", std::to_string(this->getModel()->crf).c_str());
		x265_param_parse(param_, "--preset", "8");
		x265_param_parse(param_, "--qcomp", "0.5");

		param_->bRepeatHeaders = 1;
		param_->internalCsp = X265_CSP_I420;
		param_->sourceWidth = this->width_;
		param_->sourceHeight = this->height_;
		param_->fpsNum = context->profile->timeModule->recordFps;
		param_->fpsDenom = 1;

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
	H264Component::onPostProcess() noexcept(false)
	{
		if (ostream_)
		{
			auto& context = this->getContext();

			auto output = context->profile->canvasModule->outputBuffer.data();
			this->convert((float*)output, context->profile->canvasModule->width, context->profile->canvasModule->height, this->buf_.get());

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