#include <octoon/solver_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: maxIterations_(10)
		, tolerance_(math::EPSILON)
		, time_(0)
		, timeStep_(0)
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
		maxIterations_ = iterations;
	}

	std::uint32_t
	CCDSolverComponent::getIterations() const noexcept
	{
		return maxIterations_;
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
	CCDSolverComponent::setTimeStep(float timeStep) noexcept
	{
		timeStep_ = timeStep;
	}

	float
	CCDSolverComponent::getTimeStep() const noexcept
	{
		return timeStep_;
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
		this->addComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	CCDSolverComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	CCDSolverComponent::onFrameEnd() noexcept
	{
		if (timeStep_ > 0)
		{
			auto feature = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
			if (feature)
			{
				time_ += feature->delta() * CLOCKS_PER_SEC;

				if (time_ > timeStep_)
				{
					this->evaluate();
					time_ = 0;
				}
			}
		}
		else
		{
			this->evaluate();
		}
	}

	void
	CCDSolverComponent::evaluate() noexcept
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