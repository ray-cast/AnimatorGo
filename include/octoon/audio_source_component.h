#ifndef OCTOON_AUDIO_SOURCE_COMPONENT_H_
#define OCTOON_AUDIO_SOURCE_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
    class OCTOON_EXPORT AudioSourceComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AudioSourceComponent, GameComponent)
	public:
		AudioSourceComponent() noexcept;
		~AudioSourceComponent();

		virtual GameComponentPtr clone() const noexcept;
	};
}


#endif // OCTOON_AUDIO_SOURCE_COMPONENT_H_