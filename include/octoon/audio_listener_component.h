#ifndef OCTOON_AUDIO_LISTENER_COMPONENT_H_
#define OCTOON_AUDIO_LISTENER_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/audio/common/audio_listener.h>

namespace octoon
{
    class OCTOON_EXPORT AudioListenerComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AudioListenerComponent, GameComponent)
	public:
		AudioListenerComponent() noexcept;
		virtual ~AudioListenerComponent();

		void setVolume(float volume) noexcept;
        float getVolume() const noexcept;

        void setVelocity(const math::float3& velocity) noexcept;
		void getVelocity(math::float3& velocity) const noexcept;

		virtual GameComponentPtr clone() const noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

	private:
		std::unique_ptr<AudioListener> audioListener_;
	};
}

#endif