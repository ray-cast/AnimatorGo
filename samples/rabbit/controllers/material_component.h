#ifndef RABBIT_MATERIAL_COMPONENT_H_
#define RABBIT_MATERIAL_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/material_module.h"
#include "../libs/tinyobj/tiny_obj_loader.h"

#include <map>
#include <qpixmap.h>
#include <octoon/video/renderer.h>
#include <octoon/camera/perspective_camera.h>
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

		void importMtl(std::string_view path) noexcept(false);
		void importMdl(std::string_view path) noexcept(false);

		void repaintMaterial(const std::shared_ptr<octoon::material::Material>& material, QPixmap& pixmap);

		const std::map<std::string, nlohmann::json, std::less<>>& getMaterialList() const noexcept;
		const std::shared_ptr<octoon::material::Material> getMaterial(std::string_view uuid) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		void initMaterials(std::string_view path);

	private:
		MaterialComponent(const MaterialComponent&) = delete;
		MaterialComponent& operator=(const MaterialComponent&) = delete;

	private:
		octoon::hal::GraphicsFramebufferPtr framebuffer_;
		std::map<std::string, nlohmann::json, std::less<>> materialList_;
		std::map<std::string, std::shared_ptr<octoon::material::Material>, std::less<>> materials_;

		std::shared_ptr<octoon::camera::PerspectiveCamera> camera_;
		std::shared_ptr<octoon::geometry::Geometry> geometry_;
		std::shared_ptr<octoon::light::DirectionalLight> light_;
		std::shared_ptr<octoon::light::EnvironmentLight> envlight_;
		std::shared_ptr<octoon::video::RenderScene> scene_;
	};
}

#endif