#ifndef UNREAL_RECORD_MODULE_H_
#define UNREAL_RECORD_MODULE_H_

#include <vector>
#include <unreal_model.h>
#include <octoon/math/vector3.h>

namespace unreal
{
	class RecordModule final : public UnrealModule
	{
	public:
		RecordModule() noexcept;
		virtual ~RecordModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept;

	private:
		RecordModule(const RecordModule&) = delete;
		RecordModule& operator=(const RecordModule&) = delete;

	public:
		MutableLiveData<bool> hdr;
		MutableLiveData<bool> srgb;
		MutableLiveData<bool> active;
		MutableLiveData<bool> denoise;
	};
}

#endif