#include <octoon/solver_component.h>
#include <octoon/transform_component.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>
#include <iostream>
namespace octoon
{
	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: maxIterations_(10)
		, tolerance_(0.01f)
		, time_(0)
		, timeStep_(0)
		, enableAxisLimit_(true)
	{
	}

	CCDSolverComponent::CCDSolverComponent(GameObjectPtr&& target, GameObjects&& bones) noexcept
		: CCDSolverComponent()
	{
		this->setTarget(std::move(target));
		this->setBones(std::move(bones));
	}

	CCDSolverComponent::CCDSolverComponent(const GameObjectPtr& target, const GameObjects& bones) noexcept
		: CCDSolverComponent()
	{
		this->setTarget(target);
		this->setBones(bones);
	}

	CCDSolverComponent::~CCDSolverComponent() noexcept
	{
	}

	void
	CCDSolverComponent::setTarget(const GameObjectPtr& target) noexcept
	{
		if (target_ != target)
		{
			if (target)
				this->tryAddComponentDispatch(GameDispatchType::LateUpdate);
			else
				this->tryRemoveComponentDispatch(GameDispatchType::LateUpdate);
			target_ = target;
		}
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
	CCDSolverComponent::addBone(GameObjectPtr&& bone) noexcept
	{
		bones_.emplace_back(std::move(bone));
	}

	void
	CCDSolverComponent::addBone(const GameObjectPtr& bone) noexcept
	{
		bones_.push_back(bone);
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
		if (this->getTarget())
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
			auto feature = this->getFeature<TimerFeature>();
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
				math::Vector3 localJointEnd = transform->getTransformInverse() * jointEnd;
				math::Vector3 localJointTarget = transform->getTransformInverse() * jointTarget;

				localJointEnd = math::normalize(localJointEnd);
				localJointTarget = math::normalize(localJointTarget);

				float cosDeltaAngle = math::dot(localJointTarget, localJointEnd);
				float deltaAngle = math::safe_acos(cosDeltaAngle);
				if (std::abs(deltaAngle) < math::EPSILON_E5)
					continue;

				math::Vector3 axis = math::normalize(math::cross(localJointTarget, localJointEnd));

				if (this->enableAxisLimit_)
				{
					auto limit = bone->getComponent<RotationLimitComponent>();
					if (limit)
					{
						auto angle = math::clamp(deltaAngle, limit->getMininumAngle(), limit->getMaximumAngle());
						auto spin = transform->getLocalQuaternion() * math::Quaternion(axis, angle);
						auto spinAxis = math::axis(spin);
						auto spinAngle = math::angle(spin);

						auto& low = limit->getMinimumAxis();
						auto& upper = limit->getMaximumAxis();

						if ((low.x != 0 || upper.x != 0) && low.y == 0 && upper.y == 0 && low.z == 0 && upper.z == 0)
							spin = math::Quaternion(math::float3::UnitX, math::clamp(math::sign(spinAxis.x) * spinAngle, low.x, upper.x));
						else if ((low.y != 0 || upper.y != 0) && low.x == 0 && upper.x == 0 && low.z == 0 && upper.z == 0)
							spin = math::Quaternion(math::float3::UnitY, math::clamp(math::sign(spinAxis.y) * spinAngle, low.x, upper.x));
						else if ((low.z != 0 || upper.z != 0) && low.x == 0 && upper.x == 0 && low.y == 0 && upper.y == 0)
							spin = math::Quaternion(math::float3::UnitZ, math::clamp(math::sign(spinAxis.z) * spinAngle, low.x, upper.x));
						
						transform->setLocalQuaternion(math::normalize(spin));
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
						transform->getComponent<TransformComponent>()->setLocalTranslate(additiveTranslate * rotationLimit->getAdditiveMoveRatio() + rotationLimit->getLocalTranslate());

					if (rotationLimit->getAdditiveRotationRatio() != 0.0f)
					{
						auto additiveRotation = link->getDeltaRotation(rotationLimit->getAdditiveUseLocal());
						if (!math::equal(additiveRotation, math::Quaternion::Zero))
						{
							if (rotationLimit->getAdditiveRotationRatio() > 0.0f)
							{
								auto rotation = math::slerp(math::Quaternion::Zero, additiveRotation, rotationLimit->getAdditiveRotationRatio());
								transform->setLocalQuaternion(rotation * rotationLimit->getLocalQuaternion());
							}
							else if (rotationLimit->getAdditiveRotationRatio() < 0.0f)
							{
								auto rotation = math::slerp(math::Quaternion::Zero, math::inverse(additiveRotation), -rotationLimit->getAdditiveRotationRatio());
								transform->setLocalQuaternion(rotation * rotationLimit->getLocalQuaternion());
							}
						}
					}
				}
			}
		}
	}
}