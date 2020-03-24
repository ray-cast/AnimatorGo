#ifndef RABBIT_MATERIAL_COMPONENT_H_
#define RABBIT_MATERIAL_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/material_module.h"
#include "../libs/tinyobj/tiny_obj_loader.h"

#include <map>
#include <octoon/material/material.h>

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

		void loadMaterial(std::string_view rootPath) noexcept(false);

		const std::map<std::string, nlohmann::json, std::less<>>& getMaterialList() const noexcept;
		const std::shared_ptr<octoon::material::Material> getMaterial(std::string_view uuid) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onDrop(std::string_view data) noexcept override;

	private:
		void initMaterials(std::string_view path);
		void initMaterials(const std::vector<tinyobj::material_t>& materials, std::string_view rootPath);

	private:
		MaterialComponent(const MaterialComponent&) = delete;
		MaterialComponent& operator=(const MaterialComponent&) = delete;

	private:
		std::map<std::string, nlohmann::json, std::less<>> materialList_;
		std::map<std::string, std::shared_ptr<octoon::material::Material>, std::less<>> materials_;
	};
}

#endif