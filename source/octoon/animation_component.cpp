#include <octoon/animation_component.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubClass(AnimationComponent, GameComponent, "Animation")

	AnimationComponent::AnimationComponent() noexcept
		: timer_(nullptr)
	{
	}

	AnimationComponent::AnimationComponent(animation::AnimationClip<float>&& clip) noexcept
		: timer_(nullptr)
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(animation::AnimationClips<float>&& clips) noexcept
		: timer_(nullptr)
		, clips_(std::move(clips))
	{
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClip<float>& clip) noexcept
		: timer_(nullptr)
	{
		clips_.emplace_back(clip);
	}

	AnimationComponent::AnimationComponent(const animation::AnimationClips<float>& clips) noexcept
		: timer_(nullptr)
		, clips_(clips)
	{
	}

	AnimationComponent::~AnimationComponent() noexcept
	{
	}

	void
	AnimationComponent::play() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::pause() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::stop() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	GameComponentPtr
	AnimationComponent::clone() const noexcept
	{
		return std::make_shared<AnimationComponent>();
	}

	void 
	AnimationComponent::onActivate() except
	{
		timer_ = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
	}

	void
	AnimationComponent::onDeactivate() noexcept
	{
		timer_ = nullptr;
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	AnimationComponent::onFrameBegin() except
	{
		auto delta = timer_->delta();

		for (auto& clip : clips_)
		{
			for (auto& curve : clip.curves)
			{
				curve.second.evaluate(delta);
			}
		}
	}

	void
	AnimationComponent::onFrame() except
	{
	}

	void
	AnimationComponent::onFrameEnd() except
	{
	}
}