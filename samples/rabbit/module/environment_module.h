#ifndef RABBIT_ENVIRONMENT_MODULE_H_
#define RABBIT_ENVIRONMENT_MODULE_H_

#include <rabbit_model.h>
#include <octoon/math/vector3.h>

namespace rabbit
{
	class EnvironmentModule final : public RabbitModule
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
		octoon::math::float3 color;
	};
}

#endif