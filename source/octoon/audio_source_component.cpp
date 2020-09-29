#include <octoon/audio_source_component.h>
#include <octoon/audio/openal/audio_source_al.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioSourceComponent, GameComponent, "AudioSourceComponent")

	AudioSourceComponent::AudioSourceComponent() noexcept
		:source_(std::make_shared<AudioSourceAL>())
	{
		source_->open();
	}

	AudioSourceComponent::~AudioSourceComponent() noexcept
	{
		source_->close();
	}

	void
	AudioSourceComponent::setAudioReader(std::shared_ptr<AudioReader> reader) noexcept
	{
		source_->setAudioReader(reader);
	}

	std::shared_ptr<AudioReader>
	AudioSourceComponent::getAudioReader() const noexcept
	{
		return source_->getAudioReader();
	}

	void
	AudioSourceComponent::setVolume(float volume) noexcept
	{
		source_->setVolume(volume);
	}

	void
	AudioSourceComponent::setMinVolume(float volume) noexcept 
	{
		source_->setMinVolume(volume);
	}

	void
	AudioSourceComponent::setMaxVolume(float volume) noexcept 
	{
		source_->setMaxVolume(volume);
	}

	void
	AudioSourceComponent::setVelocity(const math::float3 &velocity) noexcept 
	{
		source_->setVelocity(velocity);
	}

	void
	AudioSourceComponent::setPitch(float pitch) noexcept 
	{
		source_->setPitch(pitch);
	}

	void
	AudioSourceComponent::setMaxDistance(float maxdis) noexcept 
	{
		source_->setMaxDistance(maxdis);
	}

	void
	AudioSourceComponent::setMinDistance(float mindis) noexcept 
	{
		source_->setMinDistance(mindis);
	}
	
	void
	AudioSourceComponent::setAudioClip(const AudioClip &clip) noexcept
	{
		source_->setAudioClip(clip);
	}

	void
	AudioSourceComponent::getVelocity(math::float3 &velocity) const noexcept 
	{
		source_->getVelocity(velocity);
	}

	void
	AudioSourceComponent::getAudioClip(AudioClip &clip) const noexcept
	{
		source_->getAudioClip(clip);
	}

	float
	AudioSourceComponent::getVolume() const noexcept 
	{
		return source_->getVolume();
	}

	float
	AudioSourceComponent::getMinVolume() const noexcept 
	{
		return source_->getMinVolume();
	}

	float
	AudioSourceComponent::getMaxVolume() const noexcept 
	{
		return source_->getMaxVolume();
	}

	float
	AudioSourceComponent::getPitch() const noexcept 
	{
		return source_->getPitch();
	}

	float
	AudioSourceComponent::getMaxDistance() const noexcept 
	{
		return source_->getMaxDistance();
	}

	float
	AudioSourceComponent::getMinDistance() const noexcept 
	{
		return source_->getMinDistance();
	}

	void
	AudioSourceComponent::play(bool play) noexcept 
	{
		source_->play(play);
	}

	void
	AudioSourceComponent::loop(bool loop) noexcept 
	{
		source_->loop(loop);
	}

	void
	AudioSourceComponent::pause() noexcept 
	{
		source_->pause();
	}

	bool
	AudioSourceComponent::isPlaying() const noexcept 
	{
		return source_->isPlaying();
	}

	bool
	AudioSourceComponent::isStopped() const noexcept 
	{
		return source_->isStopped();
	}

	bool
	AudioSourceComponent::isPaused() const noexcept 
	{
		return source_->isPaused();
	}

	bool
	AudioSourceComponent::isLoop() const noexcept 
	{
		return source_->isLoop();
	}

	GameComponentPtr
	AudioSourceComponent::clone() const noexcept
	{
		math::float3 velocity;
		this->getVelocity(velocity);

		auto instance = std::make_shared<AudioSourceComponent>();
		instance->setVelocity(velocity);
		instance->setVolume(this->getVolume());
		return instance;
	}

	void
	AudioSourceComponent::onActivate() noexcept
	{
		auto transformComponent = this->getComponent<TransformComponent>();
		if (transformComponent)
		{
			auto transform = transformComponent->getTransform();
			this->source_->setTranslate(transformComponent->getTranslate());
			this->source_->setOrientation(transform.getForward(), transform.getUp());
		}

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	AudioSourceComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	AudioSourceComponent::onMoveAfter() noexcept
	{
		auto transformComponent = this->getComponent<TransformComponent>();
		auto transform = transformComponent->getTransform();
		this->source_->setTranslate(transformComponent->getTranslate());
		this->source_->setOrientation(transform.getForward(), transform.getUp());
	}
}