#include <octoon/morph_animation_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(MorphAnimationComponent, GameComponent, "MorphAnimation")

	MorphAnimationComponent::MorphAnimationComponent() noexcept
		: time_(0.0f)
		, timeStep_(1000.0f / 24.0f)
		, timeInterval_(1000.0f / 24.0f)
	{
	}

	MorphAnimationComponent::MorphAnimationComponent(math::float3s&& offsets, math::uint1s&& indices) noexcept
		: MorphAnimationComponent()
	{
		offsets_ = std::move(offsets);
		indices_ = std::move(indices);
	}

	MorphAnimationComponent::MorphAnimationComponent(const math::float3s& offsets, const math::uint1s& indices) noexcept
		: MorphAnimationComponent()
	{
		offsets_ = offsets;
		indices_ = indices;
	}

	MorphAnimationComponent::~MorphAnimationComponent() noexcept
	{
	}

	void
	MorphAnimationComponent::play() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->update();
	}

	void
	MorphAnimationComponent::pause() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MorphAnimationComponent::stop() noexcept
	{
		this->setTime(0.0f);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MorphAnimationComponent::setTime(float time) noexcept
	{
		this->update();
	}

	void
	MorphAnimationComponent::setOffsets(math::float3s&& offsets) noexcept
	{
		offsets_ = std::move(offsets);
	}

	void
	MorphAnimationComponent::setOffsets(const math::float3s& offsets) noexcept
	{
		offsets_ = offsets;
	}

	const math::float3s&
	MorphAnimationComponent::getOffsets() const noexcept
	{
		return offsets_;
	}

	void
	MorphAnimationComponent::setIndices(math::uint1s&& indices) noexcept
	{
		indices_ = std::move(indices);
	}

	void
	MorphAnimationComponent::setIndices(const math::uint1s& indices) noexcept
	{
		indices_ = indices;
	}

	const math::uint1s&
	MorphAnimationComponent::getIndices() const noexcept
	{
		return indices_;
	}

	GameComponentPtr
	MorphAnimationComponent::clone() const noexcept
	{
		return std::make_shared<MorphAnimationComponent>();
	}

	void 
	MorphAnimationComponent::onActivate() except
	{
	}

	void
	MorphAnimationComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	MorphAnimationComponent::onFixedUpdate() except
	{
		auto timeFeature = this->getGameScene()->getFeature<TimerFeature>();
		if (timeFeature)
			this->update(timeFeature->getTimeInterval());
	}

	void
	MorphAnimationComponent::update(float delta) noexcept
	{
		this->sendMessage("octoon:animation:update");
	}
}