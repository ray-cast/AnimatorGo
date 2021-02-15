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
		, needUpdatePose_(false)
		, localPose_(math::float4x4::One)
	{
    }

	SphereColliderComponent::SphereColliderComponent(float radius) noexcept
		: radius_(radius)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(radius)
		, restOffset_(0.0f)
		, needUpdatePose_(false)
		, localPose_(math::float4x4::One)
	{
	}

	SphereColliderComponent::SphereColliderComponent(float radius, float contactOffset, float restOffset) noexcept
		: radius_(radius)
		, center_(math::float3::Zero)
		, rotation_(math::Quaternion::Zero)
		, contactOffset_(contactOffset)
		, restOffset_(restOffset)
		, needUpdatePose_(false)
		, localPose_(math::float4x4::One)
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
			this->needUpdatePose_ = true;
		}
	}

	void
	SphereColliderComponent::setQuaternion(const math::Quaternion& rotation) noexcept
	{
		if (this->rotation_ != rotation)
		{
			if (shape_)
				shape_->setQuaternion(rotation);

			this->rotation_ = rotation;
			this->needUpdatePose_ = true;
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
	SphereColliderComponent::getQuaternion() const noexcept
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

	const math::float4x4&
	SphereColliderComponent::getLocalPose() const noexcept
	{
		if (needUpdatePose_)
		{
			localPose_.makeRotation(this->rotation_, this->center_);
			needUpdatePose_ = false;
		}

		return localPose_;
	}

	GameComponentPtr
	SphereColliderComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SphereColliderComponent>();
		instance->setName(this->getName());
		instance->setRadius(this->getRadius());
		instance->setCenter(this->getCenter());
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
			shape_->setQuaternion(this->getQuaternion());
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