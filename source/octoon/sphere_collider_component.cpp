#include <octoon/sphere_collider_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/physics_feature.h>

namespace octoon
{
    OctoonImplementSubClass(SphereColliderComponent, ColliderComponent, "SphereColliderComponent")

	SphereColliderComponent::SphereColliderComponent() noexcept
		: radius_(1.0)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(1.0f)
		, restOffset_(0.0f)
	{
    }

	SphereColliderComponent::SphereColliderComponent(float radius) noexcept
		: radius_(radius)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(radius)
		, restOffset_(0.0f)
	{
	}

	SphereColliderComponent::SphereColliderComponent(float radius, float contactOffset, float restOffset) noexcept
		: radius_(radius)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(contactOffset)
		, restOffset_(restOffset)
	{
		assert(contactOffset >= restOffset);
	}

    SphereColliderComponent::~SphereColliderComponent()
    {
    }

    void
	SphereColliderComponent::setRadius(float radius) noexcept
    {
		if (shape_)
			shape_->setRadius(radius);
		this->radius_ = radius;
    }

	void
	SphereColliderComponent::setCenter(const math::float3& center) noexcept
	{
		if (this->center_ != center)
		{
			if (shape_)
				shape_->setCenter(center);

			this->center_ = center;
		}
	}

	void
	SphereColliderComponent::setRotation(const math::Quaternion& rotation) noexcept
	{
		if (this->rotation_ != rotation)
		{
			if (shape_)
				shape_->setQuaternion(rotation);

			this->rotation_ = rotation;
		}
	}

	void
	SphereColliderComponent::setContactOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setContactOffset(offset);
		this->contactOffset_ = shape_->getContactOffset();
	}

	void
	SphereColliderComponent::setRestOffset(float offset) noexcept
	{
		if (shape_)
			shape_->setRestOffset(offset);
		this->restOffset_ = shape_->getRestOffset();
	}

    float
	SphereColliderComponent::getRadius() const noexcept
    {
		return this->radius_;
    }

	const math::float3& 
	SphereColliderComponent::getCenter() const noexcept
	{
		return this->center_;
	}

	float
	SphereColliderComponent::getContactOffset() const noexcept
	{
		return this->contactOffset_;
	}

	const math::Quaternion&
	SphereColliderComponent::getRotation() const noexcept
	{
		return this->rotation_;
	}

	float
	SphereColliderComponent::getRestOffset() const noexcept
	{
		return this->restOffset_;
	}

	std::shared_ptr<PhysicsShape>
	SphereColliderComponent::getShape() noexcept
	{
		return this->shape_;
	}

	math::float4x4
	SphereColliderComponent::getLocalPose() const noexcept
	{
		math::float4x4 localPose_;
		localPose_.makeRotation(this->rotation_, this->center_);
		return localPose_;
	}

	void
	SphereColliderComponent::load(const nlohmann::json& json) noexcept(false)
	{
		GameComponent::load(json);

		if (json.contains("radius"))
			this->setRadius(json["radius"]);
		if (json.contains("center"))
			this->setCenter(math::float3(json["center"].get<std::array<float, 3>>()));
		if (json.contains("rotation"))
			this->setRotation(math::Quaternion(json["rotation"].get<std::array<float, 4>>()));
		if (json.contains("contactOffset"))
			this->setContactOffset(json["contactOffset"]);
	}

	void
	SphereColliderComponent::save(nlohmann::json& json) const noexcept(false)
	{
		GameComponent::save(json);

		json["radius"] = this->getRadius();
		json["center"] = this->getCenter().to_array();
		json["rotation"] = this->getRotation().to_array();
		json["contactOffset"] = this->getContactOffset();
	}

	GameComponentPtr
	SphereColliderComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SphereColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setCenter(this->getCenter());
		instance->setRotation(this->getRotation());
		instance->setContactOffset(this->getContactOffset());
		return instance;
	}

    void
	SphereColliderComponent::onActivate() noexcept
    {
		auto physicsFeature = this->getFeature<PhysicsFeature>();
		if (physicsFeature)
		{
			PhysicsSphereShapeDesc sphereDesc;
			sphereDesc.radius = radius_;
			shape_ = physicsFeature->getContext()->createSphereShape(sphereDesc);
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
	SphereColliderComponent::onDeactivate() noexcept
    {
		shape_.reset();
		shape_ = nullptr;
    }
}