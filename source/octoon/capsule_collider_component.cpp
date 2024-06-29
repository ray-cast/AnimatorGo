#include <octoon/capsule_collider_component.h>
#include <octoon/physics_feature.h>

namespace octoon
{
    OctoonImplementSubClass(CapsuleColliderComponent, ColliderComponent, "CapsuleCollider")

	CapsuleColliderComponent::CapsuleColliderComponent() noexcept
		: radius_(1.0)
		, height_(1.0)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(0.2f)
		, restOffset_(0.0f)
    {
    }

	CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height) noexcept
		: radius_(radius)
		, height_(height)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(radius)
		, restOffset_(0)
	{
	}

	CapsuleColliderComponent::CapsuleColliderComponent(float radius, float height, float contactOffset, float restOffset) noexcept
		: radius_(radius)
		, height_(height)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(contactOffset)
		, restOffset_(restOffset)
	{
		assert(contactOffset > restOffset);
	}

    CapsuleColliderComponent::~CapsuleColliderComponent()
    {
    }

	void
	CapsuleColliderComponent::setRadius(float radius) noexcept
	{
		if (shape_)
			shape_->setRadius(radius);
		this->radius_ = radius;
	}

	void 
	CapsuleColliderComponent::setHeight(float height) noexcept
	{
		if (shape_)
			shape_->setHeight(height);
		this->height_ = height;
	}
	
	void
	CapsuleColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (this->center_ != center)
		{
			if (shape_)
				shape_->setCenter(center);

			this->center_ = center;
		}
	}

	void
	CapsuleColliderComponent::setRotation(const math::Quaternion& rotation) noexcept
	{
		if (this->rotation_ != rotation)
		{
			if (shape_)
				shape_->setQuaternion(rotation);

			this->rotation_ = rotation;
		}
	}

	float
	CapsuleColliderComponent::getRadius() const noexcept
	{
		return this->radius_;
	}

	float
	CapsuleColliderComponent::getHeight() const noexcept
	{
		return this->height_;
	}

	const math::float3& 
	CapsuleColliderComponent::getCenter() const noexcept
	{
		return this->center_;
	}

	const math::Quaternion&
	CapsuleColliderComponent::getRotation() const noexcept
	{
		return this->rotation_;
	}

	void
	CapsuleColliderComponent::setContactOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setContactOffset(offset);
		this->contactOffset_ = shape_->getContactOffset();
	}

	float
	CapsuleColliderComponent::getContactOffset() const noexcept
	{
		return this->contactOffset_;
	}

	void
	CapsuleColliderComponent::setRestOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setRestOffset(offset);
		this->restOffset_ = shape_->getRestOffset();
	}

	float
	CapsuleColliderComponent::getRestOffset() const noexcept
	{
		return this->restOffset_;
	}

	math::float4x4
	CapsuleColliderComponent::getLocalPose() const noexcept
	{
		math::float4x4 localPose_;
		localPose_.makeRotation(this->rotation_, this->center_);
		return localPose_;
	}

	void
	CapsuleColliderComponent::load(const nlohmann::json& json) noexcept(false)
	{
		GameComponent::load(json);

		if (json.contains("radius"))
			this->setRadius(json["radius"]);
		if (json.contains("height"))
			this->setHeight(json["height"]);
		if (json.contains("center"))
			this->setCenter(math::float3(json["center"].get<std::array<float, 3>>()));
		if (json.contains("rotation"))
			this->setRotation(math::Quaternion(json["rotation"].get<std::array<float, 4>>()));
		if (json.contains("contactOffset"))
			this->setContactOffset(json["contactOffset"]);
	}

	void
	CapsuleColliderComponent::save(nlohmann::json& json) const noexcept(false)
	{
		GameComponent::save(json);

		json["radius"] = this->getRadius();
		json["height"] = this->getHeight();
		json["center"] = this->getCenter().to_array();
		json["rotation"] = this->getRotation().to_array();
		json["restOffset"] = this->getRestOffset();
		json["contactOffset"] = this->getContactOffset();
	}

    GameComponentPtr
	CapsuleColliderComponent::clone() const noexcept
    {
		auto instance = std::make_shared<CapsuleColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setHeight(this->getHeight());
		instance->setCenter(this->getCenter());
		instance->setRotation(this->getRotation());
		instance->setRestOffset(this->getRestOffset());
		instance->setContactOffset(this->getContactOffset());
		return instance;
    }

	std::shared_ptr<PhysicsShape>
	CapsuleColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

    void
	CapsuleColliderComponent::onActivate() noexcept
    {
		auto physicsFeature = this->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			PhysicsCapsuleShapeDesc boxDesc;
			boxDesc.radius = radius_;
			boxDesc.height = height_;
			shape_ = physicsFeature->getContext()->createCapsuleShape(boxDesc);
			shape_->setCenter(this->center_);
			shape_->setQuaternion(this->getRotation());
			shape_->setContactOffset(this->contactOffset_);
			shape_->setRestOffset(this->restOffset_);

			this->center_ = shape_->getCenter();
			this->rotation_ = shape_->getQuaternion();
			this->restOffset_ = shape_->getRestOffset();
			this->contactOffset_ = shape_->getContactOffset();
		}
	}

    void
	CapsuleColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}