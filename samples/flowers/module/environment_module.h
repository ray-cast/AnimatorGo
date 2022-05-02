#ifndef FLOWER_ENVIRONMENT_MODULE_H_
#define FLOWER_ENVIRONMENT_MODULE_H_

#include <flower_model.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>

namespace flower
{
	class EnvironmentModule final : public FlowerModule
	{
	public:
		EnvironmentModule() noexcept;
		virtual ~EnvironmentModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		EnvironmentModule(const EnvironmentModule&) = delete;
		EnvironmentModule& operator=(const EnvironmentModule&) = delete;

	public:
		bool enable;

		float intensity;
		octoon::math::float2 offset;
		octoon::math::float3 color;
	};
}

#endif