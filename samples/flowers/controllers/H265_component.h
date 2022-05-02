#ifndef FLOWER_H265_COMPONENT_H_
#define FLOWER_H265_COMPONENT_H_

#include "module/h265_module.h"
#include "flower_component.h"

struct x265_param;
struct x265_encoder;
struct x265_picture;

namespace flower
{
	class H265Component final : public RabbitComponent<H265Module>
	{
	public:
		H265Component() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		bool record(std::string_view filepath) noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(H265Component);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept(false) override;

	private:
		void convert(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H265Component(const H265Component&) = delete;
		H265Component& operator=(const H265Component&) = delete;

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