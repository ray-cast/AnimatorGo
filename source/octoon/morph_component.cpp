#include <octoon/morph_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(MorphComponent, GameComponent, "MorphAnimation")

	MorphComponent::MorphComponent() noexcept
		: control_(0.0f)
	{
	}

	MorphComponent::MorphComponent(const std::string& name) noexcept
		: control_(0.0f)
	{
		this->setName(name);
	}

	MorphComponent::MorphComponent(math::float3s&& offsets, math::uint1s&& indices, float control) noexcept
		: MorphComponent()
	{
		offsets_ = std::move(offsets);
		indices_ = std::move(indices);
	}

	MorphComponent::MorphComponent(const math::float3s& offsets, const math::uint1s& indices, float control) noexcept
		: MorphComponent()
	{
		offsets_ = offsets;
		indices_ = indices;
	}

	MorphComponent::~MorphComponent() noexcept
	{
	}

	void
	MorphComponent::setControl(float control) noexcept
	{
		control_ = control;
	}

	float
	MorphComponent::getControl() const noexcept
	{
		return control_;
	}

	void
	MorphComponent::setOffsets(math::float3s&& offsets) noexcept
	{
		offsets_ = std::move(offsets);
	}

	void
	MorphComponent::setOffsets(const math::float3s& offsets) noexcept
	{
		offsets_ = offsets;
	}

	const math::float3s&
	MorphComponent::getOffsets() const noexcept
	{
		return offsets_;
	}

	void
	MorphComponent::setIndices(math::uint1s&& indices) noexcept
	{
		indices_ = std::move(indices);
	}

	void
	MorphComponent::setIndices(const math::uint1s& indices) noexcept
	{
		indices_ = indices;
	}

	const math::uint1s&
	MorphComponent::getIndices() const noexcept
	{
		return indices_;
	}

	GameComponentPtr
	MorphComponent::clone() const noexcept
	{
		return std::make_shared<MorphComponent>();
	}

	void 
	MorphComponent::onActivate() except
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&MorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	MorphComponent::onDeactivate() noexcept
	{
		this->removeMessageListener(this->getName(), std::bind(&MorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	MorphComponent::onAnimationUpdate(const runtime::any& control) noexcept
	{
	}

	void
	MorphComponent::update(float delta) noexcept
	{
		this->sendMessage("octoon:animation:update");
	}
}