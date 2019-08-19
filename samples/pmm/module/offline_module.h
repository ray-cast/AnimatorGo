#ifndef OCTOON_OFFLINE_MODULE_H_
#define OCTOON_OFFLINE_MODULE_H_

#include <mysticLit_model.h>
#include <vector>

namespace MysticLit
{
	class OfflineModule : public MysticLitModule
	{
	public:
		OfflineModule() noexcept;
		virtual ~OfflineModule() noexcept;

		virtual void reset() noexcept override;

	private:
		OfflineModule(const OfflineModule&) = delete;
		OfflineModule& operator=(const OfflineModule&) = delete;

	public:
		bool offlineEnable;
	};
}

#endif