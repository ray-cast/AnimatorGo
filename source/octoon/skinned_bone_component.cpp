#include <octoon/skinned_bone_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedBoneComponent, SkinnedComponent, "SkinnedBone")

	SkinnedBoneComponent::SkinnedBoneComponent() noexcept
	{
	}

	SkinnedBoneComponent::SkinnedBoneComponent(math::uint1s&& bones, math::float3s&& position, math::Quaternions&& rotation, float control) noexcept
		: SkinnedBoneComponent()
	{
		bones_ = std::move(bones);
		position_ = std::move(position);
		rotation_ = std::move(rotation);
	}

	SkinnedBoneComponent::SkinnedBoneComponent(const math::uint1s& bones, const math::float3s& position, const math::Quaternions& rotation, float control) noexcept
		: SkinnedBoneComponent()
	{
		bones_ = bones;
		position_ = position;
		rotation_ = rotation;
	}

	SkinnedBoneComponent::~SkinnedBoneComponent() noexcept
	{
	}

	void
	SkinnedBoneComponent::setPosition(math::float3s&& position) noexcept
	{
		position_ = std::move(position);
	}

	void
	SkinnedBoneComponent::setPosition(const math::float3s& position) noexcept
	{
		position_ = position;
	}

	const math::float3s&
	SkinnedBoneComponent::getPosition() const noexcept
	{
		return position_;
	}

	void
	SkinnedBoneComponent::setRotation(math::Quaternions&& rotation) noexcept
	{
		rotation_ = std::move(rotation);
	}

	void
	SkinnedBoneComponent::setRotation(const math::Quaternions& rotation) noexcept
	{
		rotation_ = rotation;
	}

	const math::Quaternions&
	SkinnedBoneComponent::getRotation() const noexcept
	{
		return rotation_;
	}

	void
	SkinnedBoneComponent::setBones(math::uint1s&& bones) noexcept
	{
		bones_ = std::move(bones);
	}

	void
	SkinnedBoneComponent::setBones(const math::uint1s& bones) noexcept
	{
		bones_ = bones;
	}

	const math::uint1s&
	SkinnedBoneComponent::getBones() const noexcept
	{
		return bones_;
	}

	GameComponentPtr
	SkinnedBoneComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedBoneComponent>();
		instance->setName(this->getName());
		instance->setControl(this->getControl());
		instance->setBones(this->getBones());
		instance->setPosition(this->getPosition());
		instance->setRotation(this->getRotation());
		return instance;
	}

	void 
	SkinnedBoneComponent::onActivate() noexcept
	{
		if (!this->getName().empty())
			this->addMessageListener(this->getName(), std::bind(&SkinnedBoneComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedBoneComponent::onDeactivate() noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedBoneComponent::onAnimationUpdate, this, std::placeholders::_1));
	}

	void
	SkinnedBoneComponent::onAnimationUpdate(const runtime::any& value) noexcept
	{
		assert(value.type() == typeid(float));
		this->setControl(runtime::any_cast<float>(value));
	}

	void
	SkinnedBoneComponent::onTargetReplace(const std::string& name) noexcept
	{
		if (!this->getName().empty())
			this->removeMessageListener(this->getName(), std::bind(&SkinnedBoneComponent::onAnimationUpdate, this, std::placeholders::_1));
		if (!name.empty())
			this->addMessageListener(name, std::bind(&SkinnedBoneComponent::onAnimationUpdate, this, std::placeholders::_1));
	}
}