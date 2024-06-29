#ifndef UNREAL_SELECTOR_MODULE_H_
#define UNREAL_SELECTOR_MODULE_H_

#include <unreal_model.h>
#include <optional>
#include <octoon/raycaster.h>

namespace unreal
{
	class SelectorModule final : public UnrealModule
	{
	public:
		SelectorModule() noexcept;
		virtual ~SelectorModule() noexcept;

		void reset() noexcept override;

		void load(nlohmann::json& reader) noexcept override;
		void save(nlohmann::json& writer) noexcept override;

		void disconnect() noexcept override;

	private:
		SelectorModule(const SelectorModule&) = delete;
		SelectorModule& operator=(const SelectorModule&) = delete;

	public:
		MutableLiveData<std::optional<octoon::RaycastHit>> selectedItem_;
		MutableLiveData<std::optional<octoon::RaycastHit>> selectedItemHover_;
	};
}

#endif