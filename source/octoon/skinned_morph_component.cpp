#include <octoon/skinned_morph_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>
#include <octoon/mesh_filter_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMorphComponent, SkinnedComponent, "SkinnedMorph")

	SkinnedMorphComponent::SkinnedMorphComponent() noexcept
		: control_(0.0f)
	{
	}

	SkinnedMorphComponent::SkinnedMorphComponent(const std::string& name) noexcept
		: control_(0.0f)
	{
		this->setName(name);
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
	SkinnedMorphComponent::setControl(float control) noexcept
	{
		control_ = control;
	}

	float
	SkinnedMorphComponent::getControl() const noexcept
	{
		return control_;
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
		return std::make_shared<SkinnedMorphComponent>();
	}

	void 
	SkinnedMorphComponent::onActivate() except
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onDeactivate() noexcept
	{
		this->removeMessageListener(this->getName(), std::bind(&SkinnedMorphComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedMorphComponent::onAnimationUpdate(const runtime::any& value) noexcept
	{
		assert(value.type() == typeid(float));
		control_ = runtime::any_cast<float>(value);
	}

	void
	SkinnedMorphComponent::update(float delta) noexcept
	{
		this->sendMessage("octoon:animation:update");
	}
}