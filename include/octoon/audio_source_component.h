#ifndef OCTOON_AUDIO_SOURCE_COMPONENT_H_
#define OCTOON_AUDIO_SOURCE_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/audio/audio_source.h>

namespace octoon
{
    class OCTOON_EXPORT AudioSourceComponent final : public GameComponent
	{
		OctoonDeclareSubClass(AudioSourceComponent, GameComponent)
	public:
		AudioSourceComponent() noexcept;
		virtual ~AudioSourceComponent();

		void play(bool isLoop = false) noexcept;
		void reset() noexcept;
		void pause() noexcept;

		void setTime(float time) noexcept;
		float getTime() const noexcept;

		float getTimeLength() const noexcept;

		void setAudioReader(std::shared_ptr<AudioReader> ptr) noexcept;
		std::shared_ptr<AudioReader> getAudioReader() const noexcept;

		void setVolume(float volume) noexcept;
		void setMinVolume(float volume) noexcept;
		void setMaxVolume(float volume) noexcept;
		void setMaxDistance(float maxdis) noexcept;
		void setMinDistance(float mindis) noexcept;
		void setVelocity(const math::float3 &velocity) noexcept;
		void setPitch(float pitch) noexcept;
		void setAudioClip(const AudioClip &clip) noexcept;

		void getVelocity(math::float3 &velocity) const noexcept;
		const AudioClip& getAudioClip() const noexcept;

		float getVolume() const noexcept;
		float getPitch() const noexcept;
		float getMinVolume() const noexcept;
		float getMaxVolume() const noexcept;
		float getMaxDistance() const noexcept;
		float getMinDistance() const noexcept;

		bool isPlaying() const noexcept;
		bool isStopped() const noexcept;
		bool isPaused() const noexcept;
		bool isLoop() const noexcept;

		virtual GameComponentPtr clone() const noexcept;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onMoveAfter() noexcept override;

	private:
		std::shared_ptr<AudioSource> source_;
		std::shared_ptr<AudioReader> audioReader_;
	};
}

#endif