#ifndef OCTOON_AUDIO_FEATURE_H_
#define OCTOON_AUDIO_FEATURE_H_

#include <memory>

#include <octoon/audio/openal/audio_device_al.h>

#include <octoon/game_feature.h>

namespace octoon
{
	class AudioFeature final : public GameFeature
	{
		OctoonDeclareSubClass(AudioFeature, GameFeature)
	public:
		AudioFeature() noexcept;
		// AudioFeature(WindHandle window, std::uint32_t w, std::uint32_t h, std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept;
		~AudioFeature() noexcept;

	private:
		virtual void on_activate() except;
		virtual void on_deactivate() noexcept;

		virtual void on_open_scene(const GameScenePtr& scene) except;
		virtual void on_close_scene(const GameScenePtr& scene) noexcept;

		virtual void on_input_event(const input::InputEvent& event) noexcept;

		virtual void on_frame_begin() except;
		virtual void on_frame() except;
		virtual void on_frame_end() except;

	private:
		std::shared_ptr<audio::AudioDeviceAL> device;
		
	};
}

#endif // OCTOON_AUDIO_FEATURE_H_