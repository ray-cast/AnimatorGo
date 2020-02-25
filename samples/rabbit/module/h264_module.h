#ifndef RABBIT_H264_MODULE_H_
#define RABBIT_H264_MODULE_H_

#include <rabbit_model.h>
#include <cstdint>

namespace rabbit
{
	enum VideoQuality
	{
		Low,
		Medium,
		High
	};

	class H264Module final : public RabbitModule
	{
	public:
		H264Module() noexcept;
		virtual ~H264Module() noexcept;

		void setVideoQuality(VideoQuality quality);

	public:
		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		H264Module(const H264Module&) = delete;
		H264Module& operator=(const H264Module&) = delete;

	public:
		bool enable;

		VideoQuality quality;

		double crf;

		std::uint32_t frame_type;
		std::uint32_t encode_speed;
	};
}

#endif