#include <octoon/configurable_joint_component.h>
#include <octoon/physics_feature.h>
#include <octoon/transform_component.h>
#include <octoon/collider_component.h>

namespace octoon
{
	OctoonImplementSubInterface(ConfigurableJointComponent, JointComponent, "ConfigurableJointComponent")

	ConfigurableJointComponent::ConfigurableJointComponent() noexcept
		: enableProjection_(false)
		, enablePreprocessing_(false)
		, motionX_(ConfigurableJointMotion::Locked)
		, motionY_(ConfigurableJointMotion::Locked)
		, motionZ_(ConfigurableJointMotion::Locked)
		, angularMotionX_(ConfigurableJointMotion::Locked)
		, angularMotionY_(ConfigurableJointMotion::Locked)
		, angularMotionZ_(ConfigurableJointMotion::Locked)
		, lowX_(0)
		, lowY_(0)
		, lowZ_(0)
		, highX_(0)
		, highY_(0)
		, highZ_(0)
		, lowAngleXLimit_(0)
		, lowAngleYLimit_(0)
		, lowAngleZLimit_(0)
		, highAngleXLimit_(0)
		, highAngleYLimit_(0)
		, highAngleZLimit_(0)
		, distanceLimit_(0.0f)
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

	const math::float3&
	ConfigurableJointComponent::getTargetPosition() const noexcept
	{
		return targetPosition_;
	}

	const math::float3&
	ConfigurableJointComponent::setTargetVelocity() const noexcept
	{
		return targetVelocity_;
	}

	const math::Quaternion&
	ConfigurableJointComponent::getTargetRotation() const noexcept
	{
		return targetRotation_;
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

			if (component)
				this->setupConfigurableJoint();
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
	ConfigurableJointComponent::setDistanceLimit(float distance) noexcept
	{
		if (joint_)
			joint_->setDistanceLimit(distance);
		this->distanceLimit_ = distance;
	}

	void
	ConfigurableJointComponent::setTwistLimit(float min, float max) noexcept
	{
		if (joint_)
			joint_->setTwistLimit(min, max);
		lowAngleXLimit_ = min;
		highAngleXLimit_ = max;
	}

	void
	ConfigurableJointComponent::setSwingLimit(float y, float z) noexcept
	{
		if (joint_)
			joint_->setSwingLimit(y, z);
		lowAngleYLimit_ = -y;
		lowAngleZLimit_ = -z;
		highAngleYLimit_ = y;
		highAngleZLimit_ = z;
	}

	void 
	ConfigurableJointComponent::setAngularLimit(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) noexcept
	{
		if (joint_)
			joint_->setAngularLimit(minX, maxX, minY, maxY, minZ, maxZ);
		lowAngleXLimit_ = minX;
		lowAngleYLimit_ = minY;
		lowAngleZLimit_ = minZ;
		highAngleXLimit_ = maxX;
		highAngleYLimit_ = maxY;
		highAngleZLimit_ = maxZ;
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
		driveAngular_.x = motion;
	}

	void
	ConfigurableJointComponent::setDriveAngularY(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveAngularY(motion);
		driveAngular_.y = motion;
	}

	void
	ConfigurableJointComponent::setDriveAngularZ(float motion) noexcept
	{
		if (joint_)
			joint_->setDriveAngularZ(motion);
		driveAngular_.z = motion;
	}

	float
	ConfigurableJointComponent::getLowXLimit() const noexcept
	{
		return lowX_;
	}

	float
	ConfigurableJointComponent::getLowYLimit() const noexcept
	{
		return lowY_;
	}

	float
	ConfigurableJointComponent::getLowZLimit() const noexcept
	{
		return lowZ_;
	}

	float
	ConfigurableJointComponent::getHighXLimit() const noexcept
	{
		return highX_;
	}

	float
	ConfigurableJointComponent::getHighYLimit() const noexcept
	{
		return highY_;
	}

	float
	ConfigurableJointComponent::getHighZLimit() const noexcept
	{
		return highZ_;
	}

	float
	ConfigurableJointComponent::getDriveMotionX() const noexcept
	{
		return driveMotion_.x;
	}

	float
	ConfigurableJointComponent::getDriveMotionY() const noexcept
	{
		return driveMotion_.y;
	}

	float
	ConfigurableJointComponent::getDriveMotionZ() const noexcept
	{
		return driveMotion_.z;
	}

	float
	ConfigurableJointComponent::getDriveAngularX() const noexcept
	{
		return driveAngular_.x;
	}

	float
	ConfigurableJointComponent::getDriveAngularY() const noexcept
	{
		return driveAngular_.y;
	}

	float
	ConfigurableJointComponent::getDriveAngularZ() const noexcept
	{
		return driveAngular_.z;
	}

	void
	ConfigurableJointComponent::getAngularLimit(float& minX, float& maxX, float& minY, float& maxY, float& minZ, float& maxZ) noexcept
	{
		minX = lowAngleXLimit_;
		minY = lowAngleYLimit_;
		minZ = lowAngleZLimit_;
		maxX = highAngleXLimit_;
		maxY = highAngleYLimit_;
		maxZ = highAngleZLimit_;
	}

	void
	ConfigurableJointComponent::enableProjection(bool enable) noexcept
	{
		if (joint_)
			joint_->enableProjection(enable);
		enableProjection_ = enable;
	}

	void
	ConfigurableJointComponent::enablePreprocessing(bool enable) noexcept
	{
		if (joint_)
			joint_->enablePreprocessing(enable);
		enablePreprocessing_ = enable;
	}

	void
	ConfigurableJointComponent::setupConfigurableJoint()
	{
		if (joint_) return;

		auto bodyA = this->getComponent<RigidbodyComponent>();
		if (bodyA && !another_.expired())
		{
			auto physicsFeature = this->tryGetFeature<PhysicsFeature>();
			if (physicsFeature)
			{
				joint_ = physicsFeature->getContext()->createConfigurableJoint(bodyA->getRigidbody(), another_.lock()->getRigidbody());
				joint_->enableProjection(this->enableProjection_);
				joint_->enablePreprocessing(this->enablePreprocessing_);
				joint_->setXMotion(motionX_);
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
				joint_->setDriveMotionX(driveMotion_.x);
				joint_->setDriveMotionY(driveMotion_.y);
				joint_->setDriveMotionZ(driveMotion_.z);
				joint_->setDriveAngularX(driveAngular_.x);
				joint_->setDriveAngularY(driveAngular_.y);
				joint_->setDriveAngularZ(driveAngular_.z);
				joint_->setAngularLimit(lowAngleXLimit_, lowAngleYLimit_, lowAngleZLimit_, highAngleXLimit_, highAngleYLimit_, highAngleZLimit_);

				this->setupConfigurableTransform(this->targetPosition_, this->targetRotation_);

				physicsFeature->getScene()->addConstraint(joint_);
			}
		}
	}

	void
	ConfigurableJointComponent::setupConfigurableTransform(const math::float3& position, const math::Quaternion& rotation)
	{
		if (joint_)
		{
			math::float4x4 transform;
			transform.makeTransform(position, rotation);

			auto transformA = math::transformMultiply(this->getComponent<TransformComponent>()->getTransform(), this->getComponent<ColliderComponent>()->getLocalPose());
			transformA = math::transformInverse(transformA);
			transformA = math::transformMultiply(transformA, transform);

			auto transformB = math::transformMultiply(another_.lock()->getComponent<TransformComponent>()->getTransform(), another_.lock()->getComponent<ColliderComponent>()->getLocalPose());
			transformB = math::transformInverse(transformB);
			transformB = math::transformMultiply(transformB, transform);

			math::float3 scaleA, scaleB;
			math::float3 translateA, translateB;
			math::Quaternion rotationA, rotationB;
			transformA.getTransform(translateA, rotationA, scaleA);
			transformB.getTransform(translateB, rotationB, scaleB);

			joint_->setFrames(translateA, rotationA, translateB, rotationB);
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
		if (joint_)
		{
			auto physicsFeature = this->tryGetFeature<PhysicsFeature>();
			if (physicsFeature)
				physicsFeature->getScene()->removeConstraint(joint_);

			joint_ = nullptr;
		}
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
			this->onDeactivate();
	}
}