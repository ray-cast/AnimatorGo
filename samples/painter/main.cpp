#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

class PainterController : public octoon::GameComponent
{
public:
	PainterController()
	{
		paths_.push_back(std::make_unique<octoon::model::Path>());
	}

	void onActivate() override
	{
		this->addComponentDispatch(octoon::GameDispatchType::Gui);

		object_ = octoon::GameObject::create();
		object_->addComponent<octoon::PathComponent>();
		object_->addComponent<octoon::MeshRendererComponent>(std::make_shared<octoon::video::LineMaterial>(2.0f));
	}

	void onDeactivate() noexcept override
	{
		this->removeComponentDispatchs();
	}

	void onGui() except override
	{
		auto transform = this->getComponent<octoon::TransformComponent>();

		auto inputFeature = this->getGameObject()->getGameScene()->getFeature<octoon::InputFeature>();
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

					paths_[0]->addEdge(camera->screenToWorld(octoon::math::float3(x, y, -1)));

					object_->getComponent<octoon::PathComponent>()->setPath(paths_);
				}
			}
		}
	}

	octoon::GameComponentPtr clone() const noexcept override
	{
		return std::make_shared<PainterController>();
	}

private:
	octoon::model::Paths paths_;
	octoon::GameObjectPtr object_;
};

int main(int argc, const char* argv[])
{
	if (!::OctoonInit(argv[0], ""))
		return 1;

	if (::OctoonOpenWindow("Octoon Studio", 1376, 768))
	{
		auto camera = octoon::GamePrefabs::instance()->createOrthoCamera();
		camera->getComponent<octoon::OrthoCameraComponent>()->setClearColor(octoon::math::float4(1.0f, 1.0f, 1.0f, 1.0));
		camera->getComponent<octoon::OrthoCameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, -200));
		camera->addComponent<PainterController>();

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}