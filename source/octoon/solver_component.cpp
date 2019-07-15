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
	SolverComponent::getTargetJoint() const noexcept
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
		auto target = this->getTargetJoint()->getComponent<TransformComponent>();
		auto effect = this->getComponent<TransformComponent>();

		for (std::uint32_t i = 0; i < this->getIterations(); i++)
		{
			auto& joints = this->getJoints();
			for (auto& joint : joints)
			{
				auto& targetPos = target->getTranslate();
				auto& effectPos = effect->getTranslate();
				if (math::distance(effectPos, targetPos) < math::EPSILON)
					return;

				auto transform = joint->bone->getComponent<TransformComponent>();
				math::Vector3 dstLocal = math::invTranslateVector3(transform->getTransform(), targetPos);
				math::Vector3 srcLocal = math::invTranslateVector3(transform->getTransform(), effectPos);

				srcLocal = math::normalize(srcLocal);
				dstLocal = math::normalize(dstLocal);

				float cosDeltaAngle = math::dot(dstLocal, srcLocal);
				if (cosDeltaAngle > 1.0 - math::EPSILON_E5)
					continue;

				float deltaAngle = math::safe_acos(cosDeltaAngle);
				deltaAngle = math::clamp(deltaAngle, joint->mininumAngle, joint->maximumAngle);

				math::Vector3 axis = math::normalize(math::cross(dstLocal, srcLocal));
				math::Quaternion q0(axis, deltaAngle);

				if (joint->enableAxisLimit)
					q0.makeRotation(math::clamp(math::eulerAngles(q0), joint->minimumRadians, joint->maximumRadians));

				transform->setLocalQuaternion(math::normalize(transform->getLocalQuaternion() * q0));
			}
		}
	}
}