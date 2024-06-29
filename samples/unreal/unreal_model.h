#ifndef UNREAL_MODEL_H_
#define UNREAL_MODEL_H_

#include <octoon/runtime/json.h>
#include "viewmodel/mutable_live_data.h"

namespace unreal
{
	class UnrealModule
	{
	public:
		UnrealModule() noexcept;
		virtual ~UnrealModule() noexcept;

		void setEnable(bool enable) noexcept;
		bool getEnable() const noexcept;

		virtual void reset() noexcept = 0;
		virtual void onValidate() noexcept;

		virtual void load(nlohmann::json& reader) noexcept(false) = 0;
		virtual void save(nlohmann::json& writer) noexcept(false) = 0;

		virtual void disconnect() noexcept;

	private:
		UnrealModule(const UnrealModule&) = delete;
		UnrealModule& operator=(const UnrealModule&) = delete;

	public:
		MutableLiveData<bool> enable;
	};
}

#endif