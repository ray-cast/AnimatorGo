#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#endif

class CubeController : public octoon::GameComponent
{
public:
	CubeController()
	{
	}

	CubeController(std::shared_ptr<octoon::video::GGXMaterial>& material)
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
		auto transform = this->getComponent<octoon::TransformComponent>();

		if (octoon::imgui::begin("Material"))
		{
			octoon::imgui::set_window_size(octoon::imgui::float2(300, 700), octoon::imgui::GuiSetCondFlagBits::FirstUseEverBit);

			if (octoon::imgui::tree_node_ex("Transform", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::math::float3 matrixTranslation = transform->getTranslate();
				octoon::math::float3 matrixRotation = octoon::math::degress(octoon::math::eulerAngles(transform->getQuaternion()));
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
				static octoon::math::float1 smoothness = 0.0f;
				static octoon::math::float1 metalness = 0.0f;
				static octoon::math::float3 lightDir = octoon::math::float3::UnitY;
				static octoon::math::float3 ambientColor(0.0f, 0.0f, 0.0f);
				static octoon::math::float3 baseColor = octoon::math::float3(31.0, 179.0, 249.0) / 255.0f;
				static octoon::math::float3 specularColor(1.0f);

				octoon::imgui::drag_float3("Light Direction", lightDir.ptr(), 0.1f);

				octoon::imgui::color_picker3("Ambient Color", ambientColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("Base Color", baseColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("Specular Color", specularColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				octoon::imgui::drag_float("smoothness", &smoothness, 0.01f, 0.0f, 1.0f);
				octoon::imgui::drag_float("metalness", &metalness, 0.01f, 0.0f, 1.0f);

				lightDir = octoon::math::normalize(lightDir);

				material_->setLightDir(lightDir);
				material_->setBaseColor(baseColor);
				material_->setSpecularColor(specularColor);
				material_->setAmbientColor(ambientColor);
				material_->setSmoothness(smoothness);
				material_->setMetalness(metalness);

				octoon::imgui::tree_pop();
			}

			octoon::imgui::end();
		}
	}

	octoon::GameComponentPtr clone() const noexcept override
	{
		return std::make_shared<CubeController>();
	}

private:
	octoon::GameObjectPtr camera_;
	std::shared_ptr<octoon::video::GGXMaterial> material_;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto camera = octoon::GameObject::create("camera");
		camera->addComponent<octoon::CameraComponent>();
		camera->addComponent<octoon::FirstPersonCameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Perspective);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 10));

		auto material = std::make_shared<octoon::video::GGXMaterial>();

		auto object = octoon::GameObject::create("actor");
		object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeCube(1.0, 1.0, 1.0));
		object->addComponent<octoon::MeshRendererComponent>(material);
		object->addComponent<octoon::GuizmoComponent>(camera);
		object->addComponent<CubeController>(material);
		
#ifdef OCTOON_BUILD_PLATFORM_EMSCRIPTEN
		// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
		emscripten_set_main_loop(OctoonUpdate, 60, 1);
#else
		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
#endif
	}

	::OctoonTerminate();
	return 0;
}