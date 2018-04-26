#include <octoon/octoon.h>

#include <octoon/video/text_material.h>
#include <octoon/game_object.h>
#include <octoon/camera_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/first_person_camera.h>

#include <octoon/ui/imgui.h>

class CubeController : public octoon::GameComponent
{
public:
	CubeController()
	{
	}

	CubeController(octoon::GameObjectPtr camera, octoon::video::TextMaterialPtr material)
		: material_(material)
		, camera_(camera)
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
		static octoon::math::float1 lern = 0.0f;
		static octoon::math::float1 extrude = 1.0f;
		static octoon::math::float3 frontColor = octoon::math::float3(31.0, 179.0, 249.0) / 255.0f;
		static octoon::math::float3 sideColor(0.0f, 1.0f, 0.0f);
		static octoon::math::float3 translate(0.0f, 0.0f, 0.0f);

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
				octoon::imgui::drag_float("lern", &lern, 0.01f, 0.0f, 1.0f);
				octoon::imgui::drag_float("extrude", &extrude, 1.0f, 0.0f, 50.0f);
				octoon::imgui::drag_float3("translate", translate.ptr(), 1.0f, 0.0f, 50.0f);

				octoon::imgui::color_picker3("front color", frontColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("side color", sideColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				material_->setLean(lern);
				material_->setExtrude(extrude);
				material_->setTextColor(octoon::video::TextColor::FrontColor, frontColor);
				material_->setTextColor(octoon::video::TextColor::SideColor, sideColor);
				material_->setTranslate(translate);

				octoon::imgui::tree_pop();
			}

			octoon::imgui::end();
		}

		auto& view = camera_->getComponent<octoon::CameraComponent>()->getView();
		auto& project = camera_->getComponent<octoon::CameraComponent>()->getProjection();
		auto model = transform->getLocalTransform();

		static octoon::imgui::guizmo::Operation op(octoon::imgui::guizmo::Rotation);
		if (octoon::imgui::is_key_pressed(octoon::input::InputKey::Code::Q))
			op = octoon::imgui::guizmo::Translate;
		if (octoon::imgui::is_key_pressed(octoon::input::InputKey::Code::W))
			op = octoon::imgui::guizmo::Rotation;
		if (octoon::imgui::is_key_pressed(octoon::input::InputKey::Code::E))
			op = octoon::imgui::guizmo::Scale;

		octoon::imgui::guizmo::BeginFrame();
		octoon::imgui::guizmo::SetRect(0, 0, octoon::imgui::get_display_size().x, octoon::imgui::get_display_size().y);
		octoon::imgui::guizmo::Manipulate(view.ptr(), project.ptr(), op, octoon::imgui::guizmo::Mode::Local, model.ptr());

		transform->setLocalTransform(model);
	}

	octoon::GameComponentPtr clone() const noexcept
	{
		return std::make_shared<CubeController>();
	}

private:
	octoon::GameObjectPtr camera_;
	octoon::video::TextMaterialPtr material_;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto material = std::make_shared<octoon::video::TextMaterial>();
		material->setLean(0.0f);
		material->setExtrude(1.0f);
		material->setTextColor(octoon::video::TextColor::FrontColor, octoon::math::float3(31.0, 179.0, 249.0) / 255.0f);
		material->setTextColor(octoon::video::TextColor::SideColor, octoon::math::float3(0.0, 1.0, 0.0));

		auto camera = std::make_shared<octoon::GameObject>();
		camera->addComponent<octoon::CameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Perspective);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->addComponent<octoon::FirstPersonCameraComponent>();
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 10));

		auto object = std::make_shared<octoon::GameObject>();
		object->addComponent<octoon::MeshFilterComponent>(std::make_shared<octoon::model::Mesh>(octoon::model::makeCube(1.0, 1.0, 1.0)));
		object->addComponent<octoon::MeshRendererComponent>(material);
		object->addComponent<CubeController>(camera, material);
		object->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3::Zero);

		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}