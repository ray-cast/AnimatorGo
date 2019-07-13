#include <octoon/solver_component.h>

namespace octoon
{
	OctoonImplementSubClass(SolverComponent, GameComponent, "Solver")

	SolverComponent::SolverComponent() noexcept
		: _chainLength(1)
	{
	}

	SolverComponent::~SolverComponent() noexcept
	{
	}

	void
	SolverComponent::setTargetBone(GameObjectPtr bone) noexcept
	{
		_target = bone;
	}

	GameObjectPtr
	SolverComponent::getTargetBone() const noexcept
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
	SolverComponent::setChainLength(std::uint32_t length) noexcept
	{
		_chainLength = length;
	}

	std::uint32_t
	SolverComponent::getChainLength() const noexcept
	{
		return _chainLength;
	}

	void
	SolverComponent::addBone(const SolverTargetPtr bone) noexcept
	{
		_bones.push_back(bone);
	}

	void
	SolverComponent::setBones(const SolverTargets& bones) noexcept
	{
		_bones = bones;
	}

	void
	SolverComponent::setBones(const SolverTargets&& bones) noexcept
	{
		_bones = std::move(bones);
	}

	const SolverComponent::SolverTargets&
	SolverComponent::getBones() const noexcept
	{
		return _bones;
	}

	GameComponentPtr
	SolverComponent::clone() const noexcept
	{
		auto iksolver = std::make_shared<SolverComponent>();
		return iksolver;
	}
}