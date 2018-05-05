#include <octoon/octoon.h>

#include <octoon/video/phong_material.h>
#include <octoon/game_object.h>
#include <octoon/camera_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/guizmo_component.h>
#include <octoon/first_person_camera.h>
#include <octoon/rigidbody_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/fixed_joint_component.h>
#include <octoon/spring_joint_component.h>

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
		static octoon::math::float1 shininess = 0.0f;
		static octoon::math::float3 lightDir = octoon::math::float3::UnitY;
		static octoon::math::float3 ambientColor(0.0f, 0.0f, 0.0f);
		static octoon::math::float3 baseColor = octoon::math::float3(31.0, 179.0, 249.0) / 255.0f;

		auto transform = this->getComponent<octoon::TransformComponent>();

		if (octoon::imgui::begin("Material"))
		{
			octoon::imgui::set_window_size(octoon::imgui::float2(300, 700), octoon::imgui::GuiSetCondFlagBits::FirstUseEverBit);

			if (octoon::imgui::tree_node_ex("Transform", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::math::float3 matrixTranslation = transform->getTranslate();
				octoon::math::float3 matrixRotation = octoon::math::degress(octoon::math::euler_angles(transform->getQuaternion()));
				octoon::math::float3 matrixScale = transform->getScale();

				octoon::imgui::drag_float3("Tr", matrixTranslation.ptr(), 3);
				octoon::imgui::drag_float3("Rt", matrixRotation.ptr(), 1);
				octoon::imgui::drag_float3("Sc", matrixScale.ptr(), 1);

				transform->setTranslate(matrixTranslation);

				transform->setQuaternion(octoon::math::Quaternion(octoon::math::radians(matrixRotation)));
				transform->setScale(matrixScale);

				octoon::imgui::tree_pop();
			}

			if (octoon::imgui::tree_node_ex("Material", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::imgui::drag_float3("Light Direction", lightDir.ptr(), 0.1f);

				octoon::imgui::color_picker3("Base color", baseColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("Ambient color", ambientColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				octoon::imgui::drag_float("Shininess", &shininess, 0.01f, 0.0f, 1.0f);

				lightDir = octoon::math::normalize(lightDir);

				material_->setLightDir(lightDir);
				material_->setBaseColor(baseColor);
				material_->setAmbientColor(ambientColor);
				material_->setShininess(shininess);

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
		camera->addComponent<octoon::FirstPersonCameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Perspective);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 10));

		auto material = std::make_shared<octoon::video::PhongMaterial>();


		std::vector<std::shared_ptr<octoon::GameObject>> domino;
		for (int i = 0; i < 8; ++i)
		{
			auto object = std::make_shared<octoon::GameObject>();
			object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeCube(1.0, 3.0, 0.2));
			object->addComponent<octoon::MeshRendererComponent>(material);
			//object->addComponent<octoon::GuizmoComponent>(camera);
			object->addComponent<CubeController>(material);
			object->addComponent<octoon::Rigidbody>(false, 1.0f, octoon::math::Vector3(0.f, 0.0f, 0.f));
			object->addComponent<octoon::BoxCollider>(octoon::math::Vector3(1.0f, 3.0f, 0.2f));
			{
				auto transform_component = object->getComponent<octoon::TransformComponent>();
				transform_component->setTranslate(octoon::math::Vector3(0.f, 5.f, 6.0 * i / 8 - 3.f));
			}
			domino.push_back(object);
		}

		auto sphere = std::make_shared<octoon::GameObject>();
		sphere->addComponent<octoon::MeshFilterComponent>(octoon::model::makeSphere(1.0f));
		sphere->addComponent<octoon::MeshRendererComponent>(material);
		sphere->addComponent<octoon::GuizmoComponent>(camera);
		//sphere->addComponent<CubeController>(material);
		sphere->addComponent<octoon::SphereCollider>(1.0f);
		sphere->addComponent<octoon::Rigidbody>(false, 1.0f, octoon::math::Vector3(0.f, 0.0f, 0.f));
		//sphere->addComponent<octoon::SpringJoint>(domino[0]->getComponent<octoon::Rigidbody>());
		{
			auto transform_component = sphere->getComponent<octoon::TransformComponent>();
			transform_component->setTranslate(octoon::math::Vector3(-2.f, 5.f, 0.f));
		}

		auto plane = std::make_shared<octoon::GameObject>();
		plane->addComponent<octoon::MeshFilterComponent>(octoon::model::makeCube(12.0, 0.5, 12.0));
		plane->addComponent<octoon::MeshRendererComponent>(material);
		//plane->addComponent<octoon::GuizmoComponent>(camera);
		plane->addComponent<octoon::Rigidbody>(true, 1.0f, octoon::math::Vector3(0.f, 0.0f, 0.f));
		plane->addComponent<octoon::BoxCollider>(octoon::math::Vector3(12.0f, 0.5f, 12.0f));
		{
			auto transform_component = plane->getComponent<octoon::TransformComponent>();
			transform_component->setTranslate(octoon::math::Vector3(0.f, -5.f, 0.f));
		}

		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}