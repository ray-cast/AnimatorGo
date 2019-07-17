#ifndef OCTOON_CCD_SOLVER_COMPONENT_H_
#define OCTOON_CCD_SOLVER_COMPONENT_H_

#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT CCDSolverComponent final : public GameComponent
	{
		OctoonDeclareSubClass(CCDSolverComponent, GameComponent)
	public:
		CCDSolverComponent() noexcept;
		~CCDSolverComponent() noexcept;

		void setTarget(const GameObjectPtr& joint) noexcept;
		const GameObjectPtr& getTarget() const noexcept;

		void setIterations(std::uint32_t iterations) noexcept;
		std::uint32_t getIterations() const noexcept;

		void setTolerance(float tolerance) noexcept;
		float getTolerance() const noexcept;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void setAxisLimitEnable(bool enable) noexcept;
		bool getAxisLimitEnable() const noexcept;

		void addBone(GameObjectPtr&& bone) noexcept;
		void addBone(const GameObjectPtr& bone) noexcept;

		void setBones(GameObjects&& joint) noexcept;
		void setBones(const GameObjects& joint) noexcept;
		const GameObjects& getBones() const noexcept;

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

		bool enableAxisLimit_;
		
		std::uint32_t maxIterations_;

		GameObjects bones_;
		GameObjectPtr target_;
	};
}

#endif