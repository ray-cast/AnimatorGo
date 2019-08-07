#include <octoon/configurable_joint_component.h>

#include <octoon/physics_feature.h>
#include <octoon/transform_component.h>
#include <octoon/game_app.h>
#include <octoon/game_scene.h>
#include <octoon/runtime/except.h>

namespace octoon
{
	OctoonImplementSubInterface(ConfigurableJointComponent, JointComponent, "ConfigurableJointComponent")

	ConfigurableJointComponent::ConfigurableJointComponent() noexcept
		: motionX_(ConfigurableJointMotion::Free)
		, motionY_(ConfigurableJointMotion::Free)
		, motionZ_(ConfigurableJointMotion::Free)
		, angularMotionX_(ConfigurableJointMotion::Free)
		, angularMotionY_(ConfigurableJointMotion::Free)
		, angularMotionZ_(ConfigurableJointMotion::Free)
		, lowX_(0)
		, lowY_(0)
		, lowZ_(0)
		, highX_(0)
		, highY_(0)
		, highZ_(0)
		, lowAngleXLimit_(-math::PI_2)
		, lowAngleYLimit_(-math::PI_2)
		, lowAngleZLimit_(-math::PI_2)
		, highAngleXLimit_(math::PI_2)
		, highAngleYLimit_(math::PI_2)
		, highAngleZLimit_(math::PI_2)
		, driveMotion_(math::float3::Zero)
		, driveAngular_(math::float3::Zero)
		, targetPosition_(math::float3::Zero)
		, targetVelocity_(math::float3::Zero)
		, targetRotation_(math::Quaternion::Zero)
	{
	}

	ConfigurableJointComponent::~ConfigurableJointComponent()
	{
	}

	void
	ConfigurableJointComponent::setTargetPosition(const math::float3& position) noexcept
	{
		if (targetPosition_ != position)
		{
			this->setupConfigurableTransform(position, targetRotation_);
			targetPosition_ = position;
		}
	}

	void 
	ConfigurableJointComponent::setTargetVelocity(const math::float3& velocity) noexcept
	{
		targetVelocity_ = velocity;
	}
	
	void
	ConfigurableJointComponent::setTargetRotation(const math::Quaternion& quat) noexcept
	{
		if (targetRotation_ != quat)
		{
			this->setupConfigurableTransform(targetPosition_, quat);
			targetRotation_ = quat;
		}
	}

	std::shared_ptr<RigidbodyComponent>
	ConfigurableJointComponent::getTarget() noexcept
	{
		return another_.lock();
	}

	void
	ConfigurableJointComponent::setTarget(std::shared_ptr<RigidbodyComponent> component) noexcept
	{
		if (another_.lock() != component)
		{
			another_ = component;

			if (joint_)
			{
				auto bodyA = this->getComponent<RigidbodyComponent>();
				joint_->connect(bodyA->getRigidbody(), another_.lock()->getRigidbody());
			}
			else if (component)
			{
				this->setupConfigurableJoint();
			}
		}		
	}

	void
	ConfigurableJointComponent::setXMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setXMotion(motion);
		this->motionX_ = motion;
	}

	void
	ConfigurableJointComponent::setYMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setYMotion(motion);
		this->motionY_ = motion;
	}

	void
	ConfigurableJointComponent::setZMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setZMotion(motion);
		this->motionZ_ = motion;
	}

	void
	ConfigurableJointComponent::setAngularXMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setAngularXMotion(motion);
		this->angularMotionX_= motion;
	}

	void
	ConfigurableJointComponent::setAngularYMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setAngularYMotion(motion);
		this->angularMotionY_ = motion;
	}

	void
	ConfigurableJointComponent::setAngularZMotion(ConfigurableJointMotion motion) noexcept
	{
		if (joint_)
			joint_->setAngularZMotion(motion);
		this->angularMotionZ_ = motion;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getXMotion() const noexcept
	{
		return this->motionX_;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getYMotion() const noexcept
	{
		return this->motionY_;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getZMotion() const noexcept
	{
		return this->motionZ_;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getAngularXMotion() const noexcept
	{
		return this->angularMotionX_;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getAngularYMotion() const noexcept
	{
		return this->angularMotionY_;
	}

	ConfigurableJointMotion
	ConfigurableJointComponent::getAngularZMotion() const noexcept
	{
		return this->angularMotionZ_;
	}

	void 
	ConfigurableJointComponent::setLowXLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setLowXLimit(limit);
		this->lowX_ = limit;
	}

	void 
	ConfigurableJointComponent::setLowYLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setLowYLimit(limit);
		this->lowY_ = limit;
	}

	void 
	ConfigurableJointComponent::setLowZLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setLowZLimit(limit);
		this->lowZ_ = limit;
	}

	void 
	ConfigurableJointComponent::setHighXLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setHighXLimit(limit);
		this->highX_ = limit;
	}

	void 
	ConfigurableJointComponent::setHighYLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setHighYLimit(limit);
		this->highY_ = limit;
	}
	void 
	ConfigurableJointComponent::setHighZLimit(float limit) noexcept
	{
		if (joint_)
			joint_->setHighZLimit(limit);
		this->highZ_ = limit;
	}

	void
	ConfigurableJointComponent::setTwistLimit(float min, float max) noexcept
	{
		if (joint_)
			joint_->setTwistLimit(min ,max);
		lowAngleXLimit_ = min;
		highAngleXLimit_ = max;
	}

	void
	ConfigurableJointComponent::setSwingLimit(float y, float z) noexcept
	{
		if (joint_)
			joint_->setSwingLimit(y, z);
		lowAngleYLimit_ = y;
		lowAngleZLimit_ = z;
	}

	void
	ConfigurableJointComponent::setPyramidSwingLimit(float min_y, float max_y, float min_z, float max_z) noexcept
	{
		if (joint_)
			joint_->setPyramidSwingLimit(min_y, max_y, min_z, max_z);
		lowAngleYLimit_ = min_y;
		lowAngleZLimit_ = min_z;
		highAngleYLimit_ = max_y;
		highAngleZLimit_ = max_z;
	}

	void
	ConfigurableJointComponent::setDriveMotionX(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveMotionX(motion);
		driveMotion_.x = motion;
	}

	void
	ConfigurableJointComponent::setDriveMotionY(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveMotionY(motion);
		driveMotion_.y = motion;
	}
	
	void
	ConfigurableJointComponent::setDriveMotionZ(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveMotionZ(motion);
		driveMotion_.z = motion;
	}

	void
	ConfigurableJointComponent::setDriveAngularX(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveAngularX(motion);
		driveAngular_.z = motion;
	}

	void
	ConfigurableJointComponent::setDriveAngularY(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveAngularY(motion);
		driveAngular_.z = motion;
	}

	void
	ConfigurableJointComponent::setDriveAngularZ(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveAngularZ(motion);
		driveAngular_.z = motion;
	}

	void
	ConfigurableJointComponent::setupConfigurableJoint()
	{
		if (joint_) return;

		auto bodyA = this->getComponent<RigidbodyComponent>();
		if (bodyA && !another_.expired())
		{
			auto physicsFeature = this->getGameScene()->getFeature<PhysicsFeature>();
			if (physicsFeature)
			{
				joint_ = physicsFeature->getContext()->createConfigurableJoint(bodyA->getRigidbody(), another_.lock()->getRigidbody());
				/*joint_->setXMotion(motionX_);
				joint_->setYMotion(motionY_);
				joint_->setZMotion(motionZ_);
				joint_->setLowXLimit(lowX_);
				joint_->setLowYLimit(lowY_);
				joint_->setLowZLimit(lowZ_);
				joint_->setHighXLimit(highX_);
				joint_->setHighYLimit(highY_);
				joint_->setHighZLimit(highZ_);
				joint_->setAngularXMotion(angularMotionX_);
				joint_->setAngularYMotion(angularMotionY_);
				joint_->setAngularZMotion(angularMotionZ_);
				joint_->setLowAngularXLimit(lowAngleXLimit_);
				joint_->setLowAngularYLimit(lowAngleYLimit_);
				joint_->setLowAngularZLimit(lowAngleZLimit_);
				joint_->setHighAngularXLimit(highAngleXLimit_);
				joint_->setHighAngularYLimit(highAngleYLimit_);
				joint_->setHighAngularZLimit(highAngleZLimit_);
				joint_->setDriveMotionX(driveMotion_.x);
				joint_->setDriveMotionY(driveMotion_.y);
				joint_->setDriveMotionZ(driveMotion_.z);
				joint_->setDriveAngularX(driveAngular_.x);
				joint_->setDriveAngularY(driveAngular_.y);
				joint_->setDriveAngularZ(driveAngular_.z);*/
			}
		}
	}

	void
	ConfigurableJointComponent::setupConfigurableTransform(const math::float3& position, const math::Quaternion& rotation)
	{
		if (joint_)
		{
			math::float4x4 transform;
			transform.makeTransform(position, math::Quaternion(rotation));

			auto transformA = this->getComponent<TransformComponent>()->getTransformInverse() * transform;
			auto transformB = another_.lock()->getComponent<TransformComponent>()->getTransformInverse() * transform;

			math::float3 scaleA, scaleB;
			math::float3 translateA, translateB;
			math::Quaternion rotationA, rotationB;
			transformA.getTransform(translateA, rotationA, scaleA);
			transformB.getTransform(translateB, rotationB, scaleB);

			joint_->setRigidATransform(translateA, rotationA);
			joint_->setRigidBTransform(translateB, rotationB);
		}
	}

	GameComponentPtr
	ConfigurableJointComponent::clone() const noexcept
	{
		auto instance = std::make_shared<ConfigurableJointComponent>();
		instance->setName(this->getName());
		instance->setXMotion(this->getXMotion());
		instance->setYMotion(this->getYMotion());
		instance->setZMotion(this->getZMotion());
		instance->setAngularXMotion(this->getAngularXMotion());
		instance->setAngularYMotion(this->getAngularYMotion());
		instance->setAngularZMotion(this->getAngularZMotion());
		return instance;
	}

	void
	ConfigurableJointComponent::onActivate() except
	{
		this->setupConfigurableJoint();
	}

	void
	ConfigurableJointComponent::onDeactivate() noexcept
	{
		joint_.reset();
	}

	void
	ConfigurableJointComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
			setupConfigurableJoint();
	}

	void
	ConfigurableJointComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<RigidbodyComponent>())
			joint_.reset();
	}
}