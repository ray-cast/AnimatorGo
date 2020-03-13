#ifndef RABBIT_MATERIAL_COMPONENT_H_
#define RABBIT_MATERIAL_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/material_module.h"
#include "../libs/tinyobj/tiny_obj_loader.h"

#include <octoon/material/material.h>
#include <octoon/runtime/any.h>

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

		void loadMaterial(const std::string_view& rootPath) noexcept(false);

		const octoon::model::Materials& getMaterials() const noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onDrop(const std::string_view& data) noexcept override;

	private:
		void initMaterials(const std::vector<tinyobj::material_t>& materials, const std::string_view& rootPath);

	private:
		MaterialComponent(const MaterialComponent&) = delete;
		MaterialComponent& operator=(const MaterialComponent&) = delete;

	private:
		octoon::model::Materials materials_;
	};
}

#endif