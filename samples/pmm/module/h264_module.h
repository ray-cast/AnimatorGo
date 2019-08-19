#ifndef OCTOON_H264_MODULE_H_
#define OCTOON_H264_MODULE_H_

#include <mysticLit_model.h>
#include <cstdint>

namespace MysticLit
{
	class H264Module : public MysticLitModule
	{
	public:
		H264Module() noexcept;
		virtual ~H264Module() noexcept;

		virtual void reset() noexcept override;

	private:
		H264Module(const H264Module&) = delete;
		H264Module& operator=(const H264Module&) = delete;

	public:
		float qp_min;
		float qp_max;

		std::uint32_t frame_type;
		std::uint32_t encode_speed;
	};
}

#endif