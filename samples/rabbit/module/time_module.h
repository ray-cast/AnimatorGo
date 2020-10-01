#ifndef RABBIT_TIME_MODULE_H_
#define RABBIT_TIME_MODULE_H_

#include <rabbit_model.h>

namespace rabbit
{
	class TimeModule final : public RabbitModule
	{
	public:
		TimeModule() noexcept;
		virtual ~TimeModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		TimeModule(const TimeModule&) = delete;
		TimeModule& operator=(const TimeModule&) = delete;

	public:
		bool playing_;
		std::uint32_t spp;
		std::uint32_t endFrame;
		std::uint32_t startFrame;
		std::uint32_t timeLength;

		float playFps;
		float playTimeStep;
		float recordFps;
		float curTime;
	};
}

#endif