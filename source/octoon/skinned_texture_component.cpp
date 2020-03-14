#include <octoon/skinned_texture_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedTextureComponent, SkinnedComponent, "SkinnedTexture")

	SkinnedTextureComponent::SkinnedTextureComponent() noexcept
	{
	}

	SkinnedTextureComponent::SkinnedTextureComponent(math::float2s&& offsets, math::uint1s&& indices, float control) noexcept
		: SkinnedTextureComponent()
	{
		offsets_ = std::move(offsets);
		indices_ = std::move(indices);
	}

	SkinnedTextureComponent::SkinnedTextureComponent(const math::float2s& offsets, const math::uint1s& indices, float control) noexcept
		: SkinnedTextureComponent()
	{
		offsets_ = offsets;
		indices_ = indices;
	}

	SkinnedTextureComponent::~SkinnedTextureComponent() noexcept
	{
	}

	void
	SkinnedTextureComponent::setOffsets(math::float2s&& offsets) noexcept
	{
		offsets_ = std::move(offsets);
	}

	void
	SkinnedTextureComponent::setOffsets(const math::float2s& offsets) noexcept
	{
		offsets_ = offsets;
	}

	const math::float2s&
	SkinnedTextureComponent::getOffsets() const noexcept
	{
		return offsets_;
	}

	void
	SkinnedTextureComponent::setIndices(math::uint1s&& indices) noexcept
	{
		indices_ = std::move(indices);
	}

	void
	SkinnedTextureComponent::setIndices(const math::uint1s& indices) noexcept
	{
		indices_ = indices;
	}

	const math::uint1s&
	SkinnedTextureComponent::getIndices() const noexcept
	{
		return indices_;
	}

	GameComponentPtr
	SkinnedTextureComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedTextureComponent>();
		instance->setName(this->getName());
		instance->setControl(this->getControl());
		instance->setOffsets(this->getOffsets());
		instance->setIndices(this->getIndices());
		return instance;
	}

	void 
	SkinnedTextureComponent::onActivate() noexcept
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&SkinnedTextureComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedTextureComponent::onDeactivate() noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedTextureComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedTextureComponent::onAnimationUpdate(const std::any& value) noexcept
	{
		assert(value.type() == typeid(float));
		this->setControl(std::any_cast<float>(value));
	}

	void
	SkinnedTextureComponent::onTargetReplace(std::string_view name) noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedTextureComponent::onAnimationUpdate, this, std::placeholders::_1));
		if (!name.empty())
			this->addMessageListener(name, std::bind(&SkinnedTextureComponent::onAnimationUpdate, this, std::placeholders::_1));
	}
}