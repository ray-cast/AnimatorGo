#ifndef OCTOON_TIME_MODULE_H_
#define OCTOON_TIME_MODULE_H_

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
	};
}

#endif