#ifndef OCTOON_AUDIO_LISTENER_COMPONENT_H_
#define OCTOON_AUDIO_LISTENER_COMPONENT_H_

#include <octoon/game_component.h>

#include <octoon/audio/openal/audio_listener_al.h>

namespace octoon
{
    class OCTOON_EXPORT AudioListenerComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AudioListenerComponent, GameComponent)
	public:
		AudioListenerComponent() noexcept;
		~AudioListenerComponent();

		void set_volume(float volume) noexcept;
        float get_volume() const noexcept;

        void set_translate(const math::float3& translate) noexcept;
        void set_velocity(const math::float3& velocity) noexcept;
        void set_orientation(const math::float3& forward, const math::float3& up) noexcept;

        void get_translate(math::float3& translate) noexcept;
        void get_velocity(math::float3& velocity) noexcept;
        void get_orientation(math::float3& forward, math::float3& up) noexcept;

		virtual GameComponentPtr clone() const noexcept;
	private:
		std::shared_ptr<audio::AudioListener> audio_listener;
	};
}


#endif // OCTOON_AUDIO_LISTENER_COMPONENT_H_