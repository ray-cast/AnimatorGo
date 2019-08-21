#ifndef MYSTICLIT_TIME_MODULE_H_
#define MYSTICLIT_TIME_MODULE_H_

#include <mysticLit_model.h>

namespace MysticLit
{
	class TimeModule : public MysticLitModule
	{
	public:
		TimeModule() noexcept;
		virtual ~TimeModule() noexcept;

		virtual void reset() noexcept override;

	private:
		TimeModule(const TimeModule&) = delete;
		TimeModule& operator=(const TimeModule&) = delete;

	public:
		float recordFps;
		float recordTimeStep;

		float playFps;
		float playTimeStep;
	};
}

#endif