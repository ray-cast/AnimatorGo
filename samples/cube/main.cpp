#include <octoon/octoon.h>

#include <octoon/video/phong_material.h>
#include <octoon/game_object.h>
#include <octoon/camera_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/guizmo_component.h>

#include <octoon/ui/imgui.h>

class CubeController : public octoon::GameComponent
{
public:
	CubeController()
	{
	}

	CubeController(std::shared_ptr<octoon::video::PhongMaterial>& material)
		: material_(material)
	{
	}

	void onActivate() override
	{
		this->addComponentDispatch(octoon::GameDispatchType::Frame);
		this->addComponentDispatch(octoon::GameDispatchType::Gui);
	}

	void onDeactivate() noexcept override
	{
		this->removeComponentDispatchs();
	}

	void onGui() except override
	{
		static octoon::math::float3 lightDir = octoon::math::float3::UnitY;
		static octoon::math::float3 ambientColor(0.0f, 0.0f, 0.0f);
		static octoon::math::float3 baseColor = octoon::math::float3(31.0, 179.0, 249.0) / 255.0f;

		auto transform = this->getComponent<octoon::TransformComponent>();

		if (octoon::imgui::begin("Material"))
		{
			octoon::imgui::set_window_size(octoon::imgui::float2(300, 700), octoon::imgui::GuiSetCondFlagBits::FirstUseEverBit);

			if (octoon::imgui::tree_node_ex("Transform", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::math::float3 matrixTranslation = transform->getLocalTranslate();
				octoon::math::float3 matrixRotation = octoon::math::degress(octoon::math::euler_angles(transform->getLocalQuaternion()));
				octoon::math::float3 matrixScale = transform->getLocalScale();

				octoon::imgui::drag_float3("Tr", matrixTranslation.ptr(), 3);
				octoon::imgui::drag_float3("Rt", matrixRotation.ptr(), 1);
				octoon::imgui::drag_float3("Sc", matrixScale.ptr(), 1);

				transform->setLocalTranslate(matrixTranslation);

				transform->setLocalQuaternion(octoon::math::Quaternion(octoon::math::radians(matrixRotation)));
				transform->setLocalScale(matrixScale);

				octoon::imgui::tree_pop();
			}

			if (octoon::imgui::tree_node_ex("Material", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::imgui::drag_float3("Light Direction", lightDir.ptr(), 0.1f);

				octoon::imgui::color_picker3("Base color", baseColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("Ambient color", ambientColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				lightDir = octoon::math::normalize(lightDir);

				material_->setLightDir(lightDir);
				material_->setBaseColor(baseColor);
				material_->setAmbientColor(ambientColor);

				octoon::imgui::tree_pop();
			}

			octoon::imgui::end();
		}
	}

	octoon::GameComponentPtr clone() const noexcept
	{
		return std::make_shared<CubeController>();
	}

private:
	octoon::GameObjectPtr camera_;
	std::shared_ptr<octoon::video::PhongMaterial> material_;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto camera = std::make_shared<octoon::GameObject>();
		camera->addComponent<octoon::CameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Perspective);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 10));

		auto material = std::make_shared<octoon::video::PhongMaterial>();

		auto object = std::make_shared<octoon::GameObject>();
		object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeCube(1.0, 1.0, 1.0));
		object->addComponent<octoon::MeshRendererComponent>(material);
		object->addComponent<octoon::GuizmoComponent>(camera);
		object->addComponent<CubeController>(material);

		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}