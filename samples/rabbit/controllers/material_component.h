#ifndef RABBIT_MATERIAL_COMPONENT_H_
#define RABBIT_MATERIAL_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/material_module.h"

namespace rabbit
{
	class MaterialComponent final : public RabbitComponent<MaterialModule>
	{
	public:
		MaterialComponent() noexcept;
		virtual ~MaterialComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(MaterialComponent);
		}

	private:
		MaterialComponent(const MaterialComponent&) = delete;
		MaterialComponent& operator=(const MaterialComponent&) = delete;
	};
}

#endif