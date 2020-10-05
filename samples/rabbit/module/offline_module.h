#ifndef RABBIT_OFFLINE_MODULE_H_
#define RABBIT_OFFLINE_MODULE_H_

#include <rabbit_model.h>

namespace rabbit
{
	class OfflineModule final : public RabbitModule
	{
	public:
		OfflineModule() noexcept;
		virtual ~OfflineModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		OfflineModule(const OfflineModule&) = delete;
		OfflineModule& operator=(const OfflineModule&) = delete;

	public:
		bool offlineEnable;
		std::uint32_t bounces;
	};
}

#endif