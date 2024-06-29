#ifndef UNREAL_SOUND_MODULE_H_
#define UNREAL_SOUND_MODULE_H_

#include <filesystem>
#include <unreal_model.h>
#include <octoon/game_object.h>

namespace unreal
{
	class SoundModule final : public UnrealModule
	{
	public:
		SoundModule() noexcept;
		virtual ~SoundModule() noexcept;

		void reset() noexcept override;

		void load(nlohmann::json& reader) noexcept override;
		void save(nlohmann::json& writer) noexcept override;

		void disconnect() noexcept override;

	private:
		SoundModule(const SoundModule&) = delete;
		SoundModule& operator=(const SoundModule&) = delete;

	public:
		MutableLiveData<float> volume;
		MutableLiveData<std::filesystem::path> filepath;
		MutableLiveData<octoon::GameObjectPtr> sound;
	};
}

#endif