#ifndef MYSTICLIT_PHYSICS_MODULE_H_
#define MYSTICLIT_PHYSICS_MODULE_H_

#include <mysticLit_model.h>
#include <cstdint>

namespace MysticLit
{
	class PhysicsModule : public MysticLitModule
	{
	public:
		PhysicsModule() noexcept;
		virtual ~PhysicsModule() noexcept;

		virtual void reset() noexcept override;

	private:
		PhysicsModule(const PhysicsModule&) = delete;
		PhysicsModule& operator=(const PhysicsModule&) = delete;

	public:
		std::uint32_t solverIterationCounts_;
	};
}

#endif