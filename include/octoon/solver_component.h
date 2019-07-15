#ifndef OCTOON_SOLVER_COMPONENT_H_
#define OCTOON_SOLVER_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT SolverJoint
	{
	public:
		GameObjectPtr bone;
		std::uint8_t enableAxisLimit;

		float mininumAngle;
		float maximumAngle;

		math::float3 minimumRadians;
		math::float3 maximumRadians;
	};

	class OCTOON_EXPORT SolverComponent final : public GameComponent
	{
		OctoonDeclareSubClass(SolverComponent, GameComponent)
	public:
		typedef std::shared_ptr<SolverJoint> SolverJointPtr;
		typedef std::vector<SolverJointPtr> SolverJoints;

	public:
		SolverComponent() noexcept;
		~SolverComponent() noexcept;

		void setTarget(GameObjectPtr joint) noexcept;
		GameObjectPtr getTarget() const noexcept;

		void setIterations(std::uint32_t iterations) noexcept;
		std::uint32_t getIterations() const noexcept;

		void addJoint(SolverJointPtr joint) noexcept;
		void setJoints(const SolverJoints& joint) noexcept;
		void setJoints(const SolverJoints&& joint) noexcept;
		const SolverJoints& getJoints() const noexcept;

		void solver() noexcept;

		GameComponentPtr clone() const noexcept;
	private:
		virtual void onActivate() noexcept override;
		virtual void onDeactivate() noexcept override;

		virtual void onMoveBefore() noexcept override;
		virtual void onMoveAfter() noexcept override;

	private:
		SolverComponent(const SolverComponent&) = delete;
		SolverComponent& operator=(const SolverComponent&) = delete;

	private:
		std::uint32_t _iterations;
		GameObjectPtr _target;
		SolverJoints _joints;
	};
}

#endif