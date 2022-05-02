#ifndef FLOWER_SUN_MODULE_H_
#define FLOWER_SUN_MODULE_H_

#include <flower_model.h>
#include <octoon/math/vector3.h>

namespace flower
{
	class SunModule final : public FlowerModule
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