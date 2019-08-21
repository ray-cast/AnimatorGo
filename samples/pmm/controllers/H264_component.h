#ifndef MYSTICLIT_H264_COMPONENT_H_
#define MYSTICLIT_H264_COMPONENT_H_

#include "module/h264_module.h"
#include "mysticLit_component.h"

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace MysticLit
{
	class H264Component final : public MysticLitComponent<H264Module>
	{
	public:
		H264Component() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void record(std::shared_ptr<std::ofstream> stream) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept override;

	private:
		static void rgb2yuv(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H264Component(const H264Component&) = delete;
		H264Component& operator=(const H264Component&) = delete;

	private:
		bool active_;

		std::uint32_t width_;
		std::uint32_t height_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		std::shared_ptr<std::ofstream> ostream_;
	};
}

#endif