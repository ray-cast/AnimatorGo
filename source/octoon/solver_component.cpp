#include <octoon/solver_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>

namespace octoon
{
	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: maxIterations_(10)
		, tolerance_(math::EPSILON)
		, time_(0)
		, timeStep_(0)
		, enableAxisLimit_(true)
		, enableRotationLink_(false)
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
		this->addComponentDispatch(GameDispatchType::LateUpdate);
	}

	void
	CCDSolverComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::LateUpdate);
	}

	void
	CCDSolverComponent::onLateUpdate() noexcept
	{
		if (timeStep_ > 0)
		{
			auto feature = this->getGameScene()->getFeature<TimerFeature>();
			if (feature)
			{
				time_ += feature->delta();

				if (time_ > timeStep_)
				{
					this->evaluateIK();
					this->evaluateRotationLink();
					time_ = 0;
				}
			}
		}
		else
		{
			this->evaluateIK();
			this->evaluateRotationLink();
		}
	}

	void
	CCDSolverComponent::evaluateIK() noexcept
	{
		if (!this->getTarget())
			return;

		this->enableRotationLink_ = false;

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
				math::Vector3 localJointEnd = math::rotate(math::inverse(transform->getQuaternion()), jointEnd - transform->getTranslate());
				math::Vector3 localJointTarget = math::rotate(math::inverse(transform->getQuaternion()), jointTarget - transform->getTranslate());

				localJointEnd = math::normalize(localJointEnd);
				localJointTarget = math::normalize(localJointTarget);

				float cosDeltaAngle = math::dot(localJointTarget, localJointEnd);
				if (math::abs(cosDeltaAngle) > 1.0 - math::EPSILON_E5)
					continue;

				this->enableRotationLink_ = true;

				float deltaAngle = math::safe_acos(cosDeltaAngle);
				math::Vector3 axis = math::normalize(math::cross(localJointTarget, localJointEnd));

				if (this->enableAxisLimit_)
				{
					auto limit = bone->getComponent<RotationLimitComponent>();
					if (limit)
					{
						math::Quaternion q0(axis, math::clamp(deltaAngle, limit->getMininumAngle(), limit->getMaximumAngle()));
						q0.makeRotation(math::clamp(math::eulerAngles(q0), limit->getMinimumAxis(), limit->getMaximumAxis()));

						transform->setLocalQuaternion(math::normalize(transform->getLocalQuaternion() * q0));
					}
					else
					{
						transform->setLocalQuaternion(math::normalize(transform->getLocalQuaternion() * math::Quaternion(axis, deltaAngle)));
					}
				}
				else
				{
					transform->setLocalQuaternion(math::normalize(transform->getLocalQuaternion() * math::Quaternion(axis, deltaAngle)));
				}
			}
		}
	}

	void
	CCDSolverComponent::evaluateRotationLink() noexcept
	{
		if (!this->enableRotationLink_)
			return;

		for (auto& it : bones_)
		{
			auto link = it->getComponent<RotationLinkComponent>();
			if (link)
			{
				for (auto& bone : link->getBones())
				{
					auto transform = bone->getComponent<TransformComponent>();
					auto rotationLimit = transform->getComponent<RotationLinkLimitComponent>();

					auto additiveTranslate = link->getDeltaTranslate(rotationLimit->getAdditiveUseLocal());
					if (rotationLimit->getAdditiveMoveRatio() != 0.0f)
					{
						transform->getComponent<TransformComponent>()->setLocalTranslateAccum(additiveTranslate * rotationLimit->getAdditiveMoveRatio());
					}

					if (rotationLimit->getAdditiveRotationRatio() != 0.0f)
					{
						auto additiveRotation = link->getDeltaRotation(rotationLimit->getAdditiveUseLocal());
						if (!math::equal(additiveRotation, math::Quaternion::Zero))
						{
							if (rotationLimit->getAdditiveRotationRatio() > 0.0f)
							{
								auto rotation = math::slerp(math::Quaternion::Zero, additiveRotation, rotationLimit->getAdditiveRotationRatio());
								transform->setLocalQuaternionAccum(rotation);
							}
							else if (rotationLimit->getAdditiveRotationRatio() < 0.0f)
							{
								auto rotation = math::slerp(math::Quaternion::Zero, math::inverse(additiveRotation), -rotationLimit->getAdditiveRotationRatio());
								transform->setLocalQuaternionAccum(rotation);
							}
						}
					}
				}
			}
		}
	}
}