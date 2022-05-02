#ifndef FLOWER_H265_MODULE_H_
#define FLOWER_H265_MODULE_H_

#include <flower_model.h>
#include <cstdint>

namespace flower
{
	enum VideoQuality
	{
		Low,
		Medium,
		High
	};

	class H265Module final : public FlowerModule
	{
	public:
		H265Module() noexcept;
		virtual ~H265Module() noexcept;

		void setVideoQuality(VideoQuality quality);

	public:
		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		H265Module(const H265Module&) = delete;
		H265Module& operator=(const H265Module&) = delete;

	public:
		bool enable;

		VideoQuality quality;

		double crf;

		std::uint32_t frame_type;
		std::uint32_t encode_speed;
	};
}

#endif