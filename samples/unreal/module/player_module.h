#ifndef UNREAL_PLAYER_MODULE_H_
#define UNREAL_PLAYER_MODULE_H_

#include <unreal_model.h>
#include <octoon/raycaster.h>
#include <octoon/game_object.h>
#include <optional>

namespace unreal
{
	class PlayerModule final : public UnrealModule
	{
	public:
		PlayerModule() noexcept;
		virtual ~PlayerModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept;

	private:
		PlayerModule(const PlayerModule&) = delete;
		PlayerModule& operator=(const PlayerModule&) = delete;

	public:
		MutableLiveData<bool> finish;
		MutableLiveData<bool> isPlaying;

		MutableLiveData<std::uint32_t> endFrame;
		MutableLiveData<std::uint32_t> startFrame;

		MutableLiveData<float> playFps;
		MutableLiveData<float> previewFps;
		MutableLiveData<float> recordFps;

		MutableLiveData<float> curTime;
		MutableLiveData<float> syncTime;
		MutableLiveData<float> takeupTime;
		MutableLiveData<float> estimatedTime;
		MutableLiveData<float> timeLength;

		std::optional<octoon::RaycastHit> dofTarget;
	};
}

#endif