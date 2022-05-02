#ifndef FLOWER_OFFLINE_MODULE_H_
#define FLOWER_OFFLINE_MODULE_H_

#include <flower_model.h>

namespace flower
{
	class OfflineModule final : public FlowerModule
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