#ifndef RABBIT_H264_COMPONENT_H_
#define RABBIT_H264_COMPONENT_H_

#include "module/h264_module.h"
#include "rabbit_component.h"

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

struct x265_param;
struct x265_encoder;
struct x265_picture;

namespace rabbit
{
	class H264Component final : public RabbitComponent<H264Module>
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

		x265_param* param_;
		x265_picture* picture_;
		x265_encoder* encoder_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		std::string filepath_;
		std::shared_ptr<std::ostream> ostream_;
	};
}

#endif