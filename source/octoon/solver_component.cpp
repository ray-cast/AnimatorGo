#include <octoon/solver_component.h>
#include <octoon/transform_component.h>
#include <octoon/timer_feature.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>

namespace octoon
{
	enum class SolveAxis
	{
		X,
		Y,
		Z,
		None
	};

	OctoonImplementSubClass(CCDSolverComponent, GameComponent, "CCDSolver")

	CCDSolverComponent::CCDSolverComponent() noexcept
		: maxIterations_(10)
		, tolerance_(0.01f)
		, time_(0)
		, timeStep_(0)
		, enableAxisLimit_(true)
		, enableAutomaticUpdate_(true)
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
			if (target && this->getAutomaticUpdate())
				this->tryAddComponentDispatch(GameDispatchType::FixedUpdate);
			else
				this->tryRemoveComponentDispatch(GameDispatchType::FixedUpdate);

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
	CCDSolverComponent::setAutomaticUpdate(bool enable) noexcept
	{
		if (this->enableAutomaticUpdate_ != enable)
		{
			this->enableAutomaticUpdate_ = enable;

			if (this->enableAutomaticUpdate_)
				this->tryAddComponentDispatch(GameDispatchType::FixedUpdate);
			else
				this->tryRemoveComponentDispatch(GameDispatchType::FixedUpdate);
		}
	}

	bool
	CCDSolverComponent::getAutomaticUpdate() const noexcept
	{
		return this->enableAutomaticUpdate_;
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

	const GameObjectPtr&
	CCDSolverComponent::getBone(std::size_t i) const noexcept
	{
		return bones_[i];
	}

	const GameObjects&
	CCDSolverComponent::getBones() const noexcept
	{
		return bones_;
	}

	void
	CCDSolverComponent::solve() noexcept
	{
		this->evaluateIK();
		this->evaluateRotationLink();
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
		instance->setAutomaticUpdate(this->getAutomaticUpdate());

		return instance;
	}

	void
	CCDSolverComponent::onActivate() noexcept
	{
		if (this->getTarget() && this->getAutomaticUpdate())
			this->addComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	CCDSolverComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	CCDSolverComponent::onFixedUpdate() noexcept
	{
		if (timeStep_ > 0)
		{
			auto feature = this->getFeature<TimerFeature>();
			if (feature)
			{
				time_ += feature->getTimeStep();

				if (time_ > timeStep_)
				{
					this->solve();
					time_ = 0;
				}
			}
		}
		else
		{
			this->solve();
		}
	}

	void
	CCDSolverComponent::evaluateIK() noexcept
	{
		auto end = this->getComponent<TransformComponent>();
		auto target = this->getTarget()->getComponent<TransformComponent>();

		for (std::uint32_t iteration = 0; iteration < this->getIterations(); iteration++)
		{
			for (auto& bone : bones_)
			{
				if (bone == this->getTarget())
					continue;

				auto& jointEnd = end->getTranslate();
				auto& jointTarget = target->getTranslate();
				if (math::sqrDistance(jointEnd, jointTarget) < tolerance_)
					return;

				auto transform = bone->getComponent<TransformComponent>();
				math::Vector3 localJointEnd = math::rotate(math::inverse(transform->getRotation()), jointEnd - transform->getTranslate());
				math::Vector3 localJointTarget = math::rotate(math::inverse(transform->getRotation()), jointTarget - transform->getTranslate());

				localJointEnd = math::normalize(localJointEnd);
				localJointTarget = math::normalize(localJointTarget);

				float cosDeltaAngle = math::dot(localJointTarget, localJointEnd);
				float deltaAngle = math::safe_acos(cosDeltaAngle);
				if (deltaAngle < 1e-5f)
					continue;

				SolveAxis solvePlane = SolveAxis::None;

				std::shared_ptr<RotationLimitComponent> limitComponent = nullptr;
				if (this->enableAxisLimit_)
				{
					limitComponent = bone->getComponent<RotationLimitComponent>();
					if (limitComponent)
					{
						auto& low = limitComponent->getMinimumAxis();
						auto& upper = limitComponent->getMaximumAxis();

						bool fuzzyZeroX = (low.x == 0 && upper.x == 0) ? true : false;
						bool fuzzyZeroY = (low.y == 0 && upper.y == 0) ? true : false;
						bool fuzzyZeroZ = (low.z == 0 && upper.z == 0) ? true : false;

						if	(	 (low.x != 0 || upper.x != 0) && fuzzyZeroY && fuzzyZeroZ)
							solvePlane = SolveAxis::X;
						else if ((low.y != 0 || upper.y != 0) && fuzzyZeroX && fuzzyZeroZ)
							solvePlane = SolveAxis::Y;
						else if ((low.z != 0 || upper.z != 0) && fuzzyZeroX && fuzzyZeroY)
							solvePlane = SolveAxis::Z;

						deltaAngle = math::clamp(deltaAngle, limitComponent->getMininumAngle(), limitComponent->getMaximumAngle());
					}
				}

				if (solvePlane != SolveAxis::None)
				{
					std::uint8_t component = 0;
					math::float3 axis(1, 0, 0);

					switch (solvePlane)
					{
					case SolveAxis::X:
						component = 0;
						axis = math::float3(1, 0, 0);
						break;
					case SolveAxis::Y:
						component = 1;
						axis = math::float3(0, 1, 0);
						break;
					case SolveAxis::Z:
						component = 2;
						axis = math::float3(0, 0, 1);
						break;
					default:
						break;
					}

					auto targetPositive = math::rotate(math::Quaternion(axis, deltaAngle), localJointTarget);
					auto targetNegative = math::rotate(math::Quaternion(axis, -deltaAngle), localJointTarget);

					auto dot1 = math::dot(targetPositive, localJointEnd);
					auto dot2 = math::dot(targetNegative, localJointEnd);

					auto newAngle = transform->getLocalEulerAngles()[component];
					newAngle += dot1 > dot2 ? deltaAngle : -deltaAngle;

					auto& low = limitComponent->getMinimumAxis();
					auto& upper = limitComponent->getMaximumAxis();

					if (iteration == 0)
					{
						if (newAngle < low[component] || newAngle > upper[component])
						{
							if (-newAngle > low[component] && -newAngle < upper[component])
								newAngle *= -1;
							else
							{
								auto halfAngle = (low[component] + upper[component]) * 0.5f;
								if (math::abs(halfAngle - newAngle) > math::abs(halfAngle + newAngle))
									newAngle *= -1;
							}
						}
					}

					math::float3 eular = math::float3::Zero;
					eular[component] = math::clamp(newAngle, low[component], upper[component]);

					transform->setLocalEulerAngles(eular);
				}
				else
				{
					math::Vector3 axis = math::normalize(math::cross(localJointTarget, localJointEnd));

					if (limitComponent)
					{
						if (limitComponent->getAxisLimitEnable())
						{
							auto& low = limitComponent->getMinimumAxis();
							auto& upper = limitComponent->getMaximumAxis();

							auto& spin = transform->getLocalEulerAngles();
							auto rotation = math::eulerAngles(math::normalize(math::Quaternion(axis, deltaAngle)));
							rotation = math::clamp(rotation, low - spin, upper - spin) + spin;

							transform->setLocalEulerAngles(rotation);
						}
						else
						{
							transform->setLocalQuaternionAccum(math::normalize(math::Quaternion(axis, deltaAngle)));
						}
					}
					else
					{
						transform->setLocalQuaternionAccum(math::normalize(math::Quaternion(axis, deltaAngle)));
					}
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
								transform->setLocalQuaternion(math::normalize(rotationLimit->getLocalQuaternion() * rotation));
							}
							else if (rotationLimit->getAdditiveRotationRatio() < 0.0f)
							{
								auto rotation = math::slerp(math::Quaternion::Zero, math::inverse(additiveRotation), -rotationLimit->getAdditiveRotationRatio());
								transform->setLocalQuaternion(math::normalize(rotationLimit->getLocalQuaternion() * rotation));
							}
						}
					}
				}
			}
		}
	}
}