#ifndef UNREAL_OFFLINE_MODULE_H_
#define UNREAL_OFFLINE_MODULE_H_

#include <unreal_model.h>

namespace unreal
{
	class OfflineModule final : public UnrealModule
	{
	public:
		OfflineModule() noexcept;
		virtual ~OfflineModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept;

	private:
		OfflineModule(const OfflineModule&) = delete;
		OfflineModule& operator=(const OfflineModule&) = delete;

	public:
		MutableLiveData<bool> showSpp;
		MutableLiveData<std::uint32_t> bounces;

		MutableLiveData<std::uint32_t> spp;
		MutableLiveData<std::uint32_t> sppCount;
	};
}

#endif