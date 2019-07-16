#ifndef OCTOON_CCD_SOLVER_COMPONENT_H_
#define OCTOON_CCD_SOLVER_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/timer_feature.h>

namespace octoon
{
	class OCTOON_EXPORT CCDJoint
	{
	public:
		GameObjectPtr bone;
		std::uint8_t enableAxisLimit;

		float mininumAngle;
		float maximumAngle;

		math::float3 minimumRadians;
		math::float3 maximumRadians;
	};

	class OCTOON_EXPORT CCDSolverComponent final : public GameComponent
	{
		OctoonDeclareSubClass(CCDSolverComponent, GameComponent)
	public:
		typedef std::shared_ptr<CCDJoint> CCDJointPtr;
		typedef std::vector<CCDJointPtr> CCDJoints;

	public:
		CCDSolverComponent() noexcept;
		~CCDSolverComponent() noexcept;

		void setTarget(GameObjectPtr joint) noexcept;
		GameObjectPtr getTarget() const noexcept;

		void setIterations(std::uint32_t iterations) noexcept;
		std::uint32_t getIterations() const noexcept;

		void setTolerance(float tolerance) noexcept;
		float getTolerance() const noexcept;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void addJoint(CCDJointPtr joint) noexcept;
		void setJoints(const CCDJoints& joint) noexcept;
		void setJoints(const CCDJoints&& joint) noexcept;
		const CCDJoints& getJoints() const noexcept;

		GameComponentPtr clone() const noexcept;
	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFrameEnd() noexcept override;

	private:
		void evaluate() noexcept;

	private:
		CCDSolverComponent(const CCDSolverComponent&) = delete;
		CCDSolverComponent& operator=(const CCDSolverComponent&) = delete;

	private:
		float time_;
		float timeStep_;
		float tolerance_;

		TimerFeature* timer_;
		
		std::uint32_t iterations_;

		GameObjectPtr target_;
		CCDJoints _joints;
	};
}

#endif