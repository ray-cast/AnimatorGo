#include <octoon/solver_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: iterations_(10)
		, tolerance_(math::EPSILON)
	{
	}

	CCDSolverComponent::~CCDSolverComponent() noexcept
	{
	}

	void
	CCDSolverComponent::setTarget(GameObjectPtr joint) noexcept
	{
		target_ = joint;
	}

	GameObjectPtr
	CCDSolverComponent::getTarget() const noexcept
	{
		return target_;
	}

	void
	CCDSolverComponent::setIterations(std::uint32_t iterations) noexcept
	{
		iterations_ = iterations;
	}

	std::uint32_t
	CCDSolverComponent::getIterations() const noexcept
	{
		return iterations_;
	}

	void
	CCDSolverComponent::setTolerance(float tolerance) noexcept
	{
		tolerance_ = tolerance;
	}

	float
	CCDSolverComponent::getTolerance() const noexcept
	{
		return tolerance_;
	}

	void
	CCDSolverComponent::addJoint(const CCDJointPtr joint) noexcept
	{
		_joints.push_back(joint);
	}

	void
	CCDSolverComponent::setJoints(const CCDJoints& joints) noexcept
	{
		_joints = joints;
	}

	void
	CCDSolverComponent::setJoints(const CCDJoints&& joints) noexcept
	{
		_joints = std::move(joints);
	}

	const CCDSolverComponent::CCDJoints&
	CCDSolverComponent::getJoints() const noexcept
	{
		return _joints;
	}

	GameComponentPtr
	CCDSolverComponent::clone() const noexcept
	{
		auto iksolver = std::make_shared<CCDSolverComponent>();
		return iksolver;
	}

	void
	CCDSolverComponent::onActivate() noexcept
	{
	}

	void
	CCDSolverComponent::onDeactivate() noexcept
	{
	}

	void
	CCDSolverComponent::onMoveBefore() noexcept
	{
	}

	void
	CCDSolverComponent::onMoveAfter() noexcept
	{
	}

	void
	CCDSolverComponent::solver() noexcept
	{
		auto end = this->getComponent<TransformComponent>();
		auto target = this->getTarget()->getComponent<TransformComponent>();

		for (std::uint32_t i = 0; i < this->getIterations(); i++)
		{
			auto& joints = this->getJoints();
			for (auto& joint : joints)
			{
				auto& jointEnd = end->getTranslate();
				auto& jointTarget = target->getTranslate();
				if (math::sqrDistance(jointEnd, jointTarget) < tolerance_)
					return;

				auto transform = joint->bone->getComponent<TransformComponent>();
				math::Vector3 localJointEnd = math::invTranslateVector3(transform->getTransform(), jointEnd);
				math::Vector3 localJointTarget = math::invTranslateVector3(transform->getTransform(), jointTarget);

				localJointEnd = math::normalize(localJointEnd);
				localJointTarget = math::normalize(localJointTarget);

				float cosDeltaAngle = math::dot(localJointTarget, localJointEnd);
				if (math::abs(cosDeltaAngle) > 1.0 - math::EPSILON_E5)
					continue;

				float deltaAngle = math::safe_acos(cosDeltaAngle);
				deltaAngle = math::clamp(deltaAngle, joint->mininumAngle, joint->maximumAngle);

				math::Vector3 axis = math::normalize(math::cross(localJointTarget, localJointEnd));
				math::Quaternion q0(axis, deltaAngle);

				if (joint->enableAxisLimit)
					q0.makeRotation(math::clamp(math::eulerAngles(q0), joint->minimumRadians, joint->maximumRadians));

				transform->setLocalQuaternion(transform->getLocalQuaternion() * q0);
			}
		}
	}
}