#include <octoon/solver_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>

namespace octoon
{
	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: maxIterations_(10)
		, tolerance_(math::EPSILON)
		, time_(0)
		, timeStep_(0)
		, enableAxisLimit_(true)
	{
	}

	CCDSolverComponent::~CCDSolverComponent() noexcept
	{
	}

	void
	CCDSolverComponent::setTarget(const GameObjectPtr& joint) noexcept
	{
		target_ = joint;
	}

	const GameObjectPtr&
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
	CCDSolverComponent::setAxisLimitEnable(bool enable) noexcept
	{
		enableAxisLimit_ = enable;
	}

	bool
	CCDSolverComponent::getAxisLimitEnable() const noexcept
	{
		return enableAxisLimit_;
	}

	void
	CCDSolverComponent::addBone(GameObjectPtr&& joint) noexcept
	{
		bones_.emplace_back(std::move(joint));
	}

	void
	CCDSolverComponent::addBone(const GameObjectPtr& joint) noexcept
	{
		bones_.push_back(joint);
	}

	void
	CCDSolverComponent::setBones(GameObjects&& bones) noexcept
	{
		bones_ = std::move(bones);
	}

	void
	CCDSolverComponent::setBones(const GameObjects& bones) noexcept
	{
		bones_ = bones;
	}

	const GameObjects&
	CCDSolverComponent::getBones() const noexcept
	{
		return bones_;
	}

	GameComponentPtr
	CCDSolverComponent::clone() const noexcept
	{
		auto instance = std::make_shared<CCDSolverComponent>();
		instance->setName(this->getName());
		instance->setTarget(this->getTarget());
		instance->setBones(this->getBones());
		instance->setIterations(this->getIterations());
		instance->setTimeStep(this->getTimeStep());
		instance->setTolerance(this->getTolerance());
		instance->setAxisLimitEnable(this->getAxisLimitEnable());

		return instance;
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
					this->evaluate2();
					time_ = 0;
				}
			}
		}
		else
		{
			this->evaluate();
			this->evaluate2();
		}
	}

	void
	CCDSolverComponent::evaluate() noexcept
	{
		if (!this->getTarget())
			return;

		auto end = this->getComponent<TransformComponent>();
		auto target = this->getTarget()->getComponent<TransformComponent>();

		for (std::uint32_t i = 0; i < this->getIterations(); i++)
		{
			for (auto& bone : bones_)
			{
				auto& jointEnd = end->getTranslate();
				auto& jointTarget = target->getTranslate();
				if (math::sqrDistance(jointEnd, jointTarget) < tolerance_)
					return;

				auto transform = bone->getComponent<TransformComponent>();
				math::Vector3 localJointEnd = math::invTranslateVector3(transform->getTransform(), jointEnd);
				math::Vector3 localJointTarget = math::invTranslateVector3(transform->getTransform(), jointTarget);

				localJointEnd = math::normalize(localJointEnd);
				localJointTarget = math::normalize(localJointTarget);

				float cosDeltaAngle = math::dot(localJointTarget, localJointEnd);
				if (math::abs(cosDeltaAngle) > 1.0 - math::EPSILON_E5)
					continue;

				float deltaAngle = math::safe_acos(cosDeltaAngle);
				math::Vector3 axis = math::normalize(math::cross(localJointTarget, localJointEnd));

				if (this->enableAxisLimit_)
				{
					auto limit = bone->getComponent<RotationLimitComponent>();
					if (limit)
					{
						math::Quaternion q0(axis, math::clamp(deltaAngle, limit->getMininumAngle(), limit->getMaximumAngle()));
						q0.makeRotation(math::clamp(math::eulerAngles(q0), limit->getMinimumAxis(), limit->getMaximumAxis()));

						transform->setLocalQuaternion(transform->getLocalQuaternion() * q0);
					}
					else
					{
						transform->setLocalQuaternion(transform->getLocalQuaternion() * math::Quaternion(axis, deltaAngle));
					}
				}
				else
				{
					transform->setLocalQuaternion(transform->getLocalQuaternion() * math::Quaternion(axis, deltaAngle));
				}	
			}
		}
	}

	void
	CCDSolverComponent::evaluate2() noexcept
	{
		for (auto& bone : bones_)
		{
			auto boneController = bone->getComponent<RotationLinkComponent>();
			if (boneController)
				boneController->solve();
		}
	}
}