#include <octoon/octoon.h>

#include <octoon/game_app.h>
#include <octoon/game_object.h>
#include <octoon/video/line_material.h>
#include <octoon/camera_component.h>
#include <octoon/model/text_meshing.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/guizmo_component.h>
#include <octoon/first_person_camera_component.h>
#include <octoon/ui/imgui.h>
#include <octoon/input_feature.h>

class PainterController : public octoon::GameComponent
{
public:
	PainterController()
	{
		contours_.push_back(std::make_unique<octoon::model::Contour>());
	}

	void onActivate() override
	{
		this->addComponentDispatch(octoon::GameDispatchType::Gui);

		auto material_ = std::make_shared<octoon::video::LineMaterial>(2);

		object_ = octoon::GameObject::create();
		object_->addComponent<octoon::MeshFilterComponent>();
		object_->addComponent<octoon::MeshRendererComponent>(material_);
		object_->addComponent<octoon::GuizmoComponent>(this->getGameObject());
	}

	void onDeactivate() noexcept override
	{
		this->removeComponentDispatchs();
	}

	void onGui() except override
	{
		auto transform = this->getComponent<octoon::TransformComponent>();

		auto inputFeature = octoon::GameApp::instance()->getFeature<octoon::InputFeature>();
		if (inputFeature)
		{
			auto input = inputFeature->getInput();
			if (input)
			{
				if (input->isButtonDown(octoon::input::InputButton::Left))
				{
					float x, y;
					input->getMousePos(x, y);

					auto camera = this->getComponent<octoon::CameraComponent>();

					contours_[0]->addPoints(camera->screenToWorld(octoon::math::float3(x, y, -1)));

					object_->getComponent<octoon::MeshFilterComponent>()->setMesh(octoon::model::makeMeshWireframe(contours_));
				}
			}
		}
	}

	octoon::GameComponentPtr clone() const noexcept
	{
		return std::make_shared<PainterController>();
	}

private:
	octoon::model::Contours contours_;
	octoon::GameObjectPtr object_;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto camera = octoon::GameObject::create();
		camera->addComponent<octoon::CameraComponent>();
		camera->addComponent<octoon::FirstPersonCameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(1.0f, 1.0f, 1.0f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Ortho);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 200));
		camera->addComponent<PainterController>();

		while (!::OctoonIsQuitRequest())
			::OctoonUpdate();
	}

	::OctoonTerminate();
	return 0;
}