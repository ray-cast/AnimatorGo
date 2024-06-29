#ifndef UNREAL_ENVIRONMENT_COMPONENT_H_
#define UNREAL_ENVIRONMENT_COMPONENT_H_

#include "../unreal_component.h"
#include "../module/environment_module.h"

namespace unreal
{
	class EnvironmentComponent final : public UnrealComponent<EnvironmentModule>
	{
	public:
		EnvironmentComponent() noexcept;
		virtual ~EnvironmentComponent() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(EnvironmentComponent);
		}

	private:
		void onInit() noexcept override;

		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		EnvironmentComponent(const EnvironmentComponent&) = delete;
		EnvironmentComponent& operator=(const EnvironmentComponent&) = delete;

	private:
		std::shared_ptr<octoon::Texture> texture_;
		std::shared_ptr<octoon::Texture> radianceTexture_;
	};
}

#endif