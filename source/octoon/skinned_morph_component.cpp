#include <octoon/skinned_morph_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMorphComponent, SkinnedComponent, "SkinnedMorph")

	SkinnedMorphComponent::SkinnedMorphComponent() noexcept
	{
	}

	SkinnedMorphComponent::SkinnedMorphComponent(math::float3s&& offsets, math::uint1s&& indices, float control) noexcept
		: SkinnedMorphComponent()
	{
		offsets_ = std::move(offsets);
		indices_ = std::move(indices);
	}

	SkinnedMorphComponent::SkinnedMorphComponent(const math::float3s& offsets, const math::uint1s& indices, float control) noexcept
		: SkinnedMorphComponent()
	{
		offsets_ = offsets;
		indices_ = indices;
	}

	SkinnedMorphComponent::~SkinnedMorphComponent() noexcept
	{
	}

	void
	SkinnedMorphComponent::setOffsets(math::float3s&& offsets) noexcept
	{
		offsets_ = std::move(offsets);
	}

	void
	SkinnedMorphComponent::setOffsets(const math::float3s& offsets) noexcept
	{
		offsets_ = offsets;
	}

	const math::float3s&
	SkinnedMorphComponent::getOffsets() const noexcept
	{
		return offsets_;
	}

	void
	SkinnedMorphComponent::setIndices(math::uint1s&& indices) noexcept
	{
		indices_ = std::move(indices);
	}

	void
	SkinnedMorphComponent::setIndices(const math::uint1s& indices) noexcept
	{
		indices_ = indices;
	}

	const math::uint1s&
	SkinnedMorphComponent::getIndices() const noexcept
	{
		return indices_;
	}

	GameComponentPtr
	SkinnedMorphComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedMorphComponent>();
		instance->setName(this->getName());
		instance->setControl(this->getControl());
		instance->setOffsets(this->getOffsets());
		instance->setIndices(this->getIndices());
		return instance;
	}

	void 
	SkinnedMorphComponent::onActivate() noexcept
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onDeactivate() noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onAnimationUpdate(const std::any& value) noexcept
	{
		assert(value.type() == typeid(float));
		this->setControl(std::any_cast<float>(value));
	}

	void
	SkinnedMorphComponent::onTargetReplace(std::string_view name) noexcept
	{
		if (!this->getName().empty())
			this->tryRemoveMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
		if (!name.empty())
			this->tryAddMessageListener(name, std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}
}