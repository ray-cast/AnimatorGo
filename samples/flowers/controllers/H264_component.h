#ifndef RABBIT_H264_COMPONENT_H_
#define RABBIT_H264_COMPONENT_H_

#include "module/h265_module.h"
#include "rabbit_component.h"

struct x264_t;
struct x264_picture_t;

namespace rabbit
{
	class H264Component final : public RabbitComponent<H265Module>
	{
	public:
		H264Component() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		bool record(std::string_view filepath) noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(H264Component);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept(false) override;

	private:
		void convert(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H264Component(const H264Component&) = delete;
		H264Component& operator=(const H264Component&) = delete;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		x264_t* encoder_;
		x264_picture_t* frame_;
		x264_picture_t* encoded_frame_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		std::string filepath_;
		std::shared_ptr<std::ostream> ostream_;
	};
}

#endif