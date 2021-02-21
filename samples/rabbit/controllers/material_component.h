#ifndef RABBIT_MATERIAL_COMPONENT_H_
#define RABBIT_MATERIAL_COMPONENT_H_

#include "../rabbit_component.h"
#include "../module/material_module.h"

#include <set>
#include <map>
#include <optional>
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

		void importMdl(std::string_view path) noexcept(false);

		void repaintMaterial(const std::shared_ptr<octoon::Material>& material, QPixmap& pixmap, int w, int h);

		std::optional<std::string> getSelectedMaterial();

		const std::map<std::string, nlohmann::json, std::less<>>& getMaterialList() const noexcept;
		const std::shared_ptr<octoon::Material> getMaterial(std::string_view uuid) noexcept;

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

		std::set<void*> materialSets_;
		std::map<std::string, nlohmann::json, std::less<>> materialList_;
		std::map<std::string, octoon::MaterialPtr, std::less<>> materials_;
		std::map<octoon::MaterialPtr, std::string, std::less<>> materialsRemap_;

		std::optional<std::string> selectedMaterial_;

		std::shared_ptr<octoon::PerspectiveCamera> camera_;
		std::shared_ptr<octoon::Geometry> geometry_;
		std::shared_ptr<octoon::DirectionalLight> light_;
		std::shared_ptr<octoon::EnvironmentLight> envlight_;
		std::shared_ptr<octoon::RenderScene> scene_;
	};
}

#endif