#ifndef UNREAL_PHYSICS_MODULE_H_
#define UNREAL_PHYSICS_MODULE_H_

#include <unreal_model.h>
#include <cstdint>
#include <octoon/math/vector3.h>

namespace unreal
{
	class PhysicsModule final : public UnrealModule
	{
	public:
		PhysicsModule() noexcept;
		virtual ~PhysicsModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept override;

	private:
		PhysicsModule(const PhysicsModule&) = delete;
		PhysicsModule& operator=(const PhysicsModule&) = delete;

	public:
		MutableLiveData<octoon::math::float3> gravity;

		MutableLiveData<float> fixedTimeStep;
		MutableLiveData<float> gravityScale;

		MutableLiveData<std::uint32_t> playSolverIterationCounts;
		MutableLiveData<std::uint32_t> recordSolverIterationCounts;
		MutableLiveData<std::uint32_t> previewSolverIterationCounts;
	};
}

#endif