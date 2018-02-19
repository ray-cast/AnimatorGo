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

		virtual GameComponentPtr clone() const noexcept;

	private:
	};
}


#endif // OCTOON_AUDIO_LISTENER_COMPONENT_H_