#include <octoon/audio_listener_component.h>
#include <octoon/transform_component.h>
#include <octoon/audio/openal/audio_listener_al.h>

namespace octoon
{
	OctoonImplementSubInterface(AudioListenerComponent, GameComponent, "AudioListenerComponent")

	AudioListenerComponent::AudioListenerComponent() noexcept
		: audioListener_(std::make_unique<AudioListenerAL>())
	{
	}

	AudioListenerComponent::~AudioListenerComponent() noexcept
	{
	}

	void
	AudioListenerComponent::setVolume(float volume) noexcept
	{
		audioListener_->setVolume(volume);
	}

	float
	AudioListenerComponent::getVolume() const noexcept
	{
		return audioListener_->getVolume();
	}

	void
	AudioListenerComponent::setVelocity(const math::float3 &velocity) noexcept
	{
		audioListener_->setVelocity(velocity);
	}

	void
	AudioListenerComponent::getVelocity(math::float3 &velocity) const noexcept
	{
		audioListener_->getVelocity(velocity);
	}

	GameComponentPtr
	AudioListenerComponent::clone() const noexcept
	{
		math::float3 velocity;
		this->getVelocity(velocity);

		auto instance = std::make_shared<AudioListenerComponent>();
		instance->setVelocity(velocity);
		instance->setVolume(this->getVolume());
		return instance;
	}

	void
	AudioListenerComponent::onActivate() noexcept
	{
		auto transformComponent = this->getComponent<TransformComponent>();
		if (transformComponent)
		{
			auto transform = transformComponent->getTransform();
			this->audioListener_->setTranslate(transformComponent->getTranslate());
			this->audioListener_->setOrientation(transform.getForward(), transform.getUp());
		}

		this->addComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	AudioListenerComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
	}

	void
	AudioListenerComponent::onMoveAfter() noexcept
	{
		auto transformComponent = this->getComponent<TransformComponent>();
		auto transform = transformComponent->getTransform();
		this->audioListener_->setTranslate(transformComponent->getTranslate());
		this->audioListener_->setOrientation(transform.getForward(), transform.getUp());
	}
}