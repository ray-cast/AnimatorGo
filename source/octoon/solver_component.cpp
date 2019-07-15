#include <octoon/solver_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(SolverComponent, GameComponent, "Solver")

	SolverComponent::SolverComponent() noexcept
		: _iterations(10)
	{
	}

	SolverComponent::~SolverComponent() noexcept
	{
	}

	void
	SolverComponent::setTarget(GameObjectPtr joint) noexcept
	{
		_target = joint;
	}

	GameObjectPtr
	SolverComponent::getTarget() const noexcept
	{
		return _target;
	}

	void
	SolverComponent::setIterations(std::uint32_t iterations) noexcept
	{
		_iterations = iterations;
	}

	std::uint32_t
	SolverComponent::getIterations() const noexcept
	{
		return _iterations;
	}

	void
	SolverComponent::addJoint(const SolverJointPtr joint) noexcept
	{
		_joints.push_back(joint);
	}

	void
	SolverComponent::setJoints(const SolverJoints& joints) noexcept
	{
		_joints = joints;
	}

	void
	SolverComponent::setJoints(const SolverJoints&& joints) noexcept
	{
		_joints = std::move(joints);
	}

	const SolverComponent::SolverJoints&
	SolverComponent::getJoints() const noexcept
	{
		return _joints;
	}

	GameComponentPtr
	SolverComponent::clone() const noexcept
	{
		auto iksolver = std::make_shared<SolverComponent>();
		return iksolver;
	}

	void
	SolverComponent::onActivate() noexcept
	{
	}

	void
	SolverComponent::onDeactivate() noexcept
	{
	}

	void
	SolverComponent::onMoveBefore() noexcept
	{
	}

	void
	SolverComponent::onMoveAfter() noexcept
	{
	}

	void
	SolverComponent::solver() noexcept
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
				if (math::sqrDistance(jointEnd, jointTarget) < math::EPSILON)
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