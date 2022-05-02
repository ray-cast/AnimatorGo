#ifndef RABBIT_SUN_MODULE_H_
#define RABBIT_SUN_MODULE_H_

#include <rabbit_model.h>
#include <octoon/math/vector3.h>

namespace rabbit
{
	class SunModule final : public RabbitModule
	{
	public:
		SunModule() noexcept;
		virtual ~SunModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		SunModule(const SunModule&) = delete;
		SunModule& operator=(const SunModule&) = delete;

	public:
		bool enable;

		float intensity;
		octoon::math::float3 color;
		octoon::math::float3 rotation;
	};
}

#endif