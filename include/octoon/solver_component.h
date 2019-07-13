#ifndef OCTOON_SOLVER_COMPONENT_H_
#define OCTOON_SOLVER_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT SolverTarget
	{
	public:
		GameObjectPtr bone;
		std::uint8_t rotateLimited;

		float angleWeight;

		math::float3 minimumDegrees;
		math::float3 maximumDegrees;
	};

	class OCTOON_EXPORT SolverComponent final : public GameComponent
	{
		OctoonDeclareSubClass(SolverComponent, GameComponent)
	public:
		typedef std::shared_ptr<SolverTarget> SolverTargetPtr;
		typedef std::vector<SolverTargetPtr> SolverTargets;

	public:
		SolverComponent() noexcept;
		~SolverComponent() noexcept;

		void setTargetBone(GameObjectPtr bone) noexcept;
		GameObjectPtr getTargetBone() const noexcept;

		void setIterations(std::uint32_t iterations) noexcept;
		std::uint32_t getIterations() const noexcept;

		void setChainLength(std::uint32_t length) noexcept;
		std::uint32_t getChainLength() const noexcept;

		void addBone(SolverTargetPtr bone) noexcept;
		void setBones(const SolverTargets& bone) noexcept;
		void setBones(const SolverTargets&& bone) noexcept;
		const SolverTargets& getBones() const noexcept;

		GameComponentPtr clone() const noexcept;

	private:
		SolverComponent(const SolverComponent&) = delete;
		SolverComponent& operator=(const SolverComponent&) = delete;

	private:
		GameObjectPtr _target;

		std::uint32_t _iterations;
		std::uint32_t _chainLength;

		SolverTargets _bones;
	};
}

#endif