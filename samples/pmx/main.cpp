#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

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
			octoon::imgui::setWindowSize(octoon::imgui::float2(300, 700), octoon::imgui::GuiSetCondFlagBits::FirstUseEverBit);

			if (octoon::imgui::treeNodeEx("Transform", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				octoon::math::float3 matrixTranslation = transform->getTranslate();
				octoon::math::float3 matrixRotation = octoon::math::degress(octoon::math::eulerAngles(transform->getQuaternion()));
				octoon::math::float3 matrixScale = transform->getScale();

				octoon::imgui::dragFloat3("Tr", matrixTranslation.ptr(), 3);
				octoon::imgui::dragFloat3("Rt", matrixRotation.ptr(), 1);
				octoon::imgui::dragFloat3("Sc", matrixScale.ptr(), 1);

				transform->setTranslate(matrixTranslation);

				transform->setQuaternion(octoon::math::Quaternion(octoon::math::radians(matrixRotation)));
				transform->setScale(matrixScale);

				octoon::imgui::treePop();
			}

			if (material_ && octoon::imgui::treeNodeEx("Material", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				static octoon::math::float1 smoothness = 0.0f;
				static octoon::math::float1 metalness = 0.0f;
				static octoon::math::float3 lightDir = -octoon::math::float3::UnitY;
				static octoon::math::float3 ambientColor(0.0f, 0.0f, 0.0f);
				static octoon::math::float3 baseColor = octoon::math::float3(62.0f, 62.0f, 62.0f) / 255.0f;
				static octoon::math::float3 specularColor(1.0f);

				octoon::imgui::dragFloat3("Light Direction", lightDir.ptr(), 0.1f);

				octoon::imgui::colorPicker3("Ambient Color", ambientColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::colorPicker3("Base Color", baseColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::colorPicker3("Specular Color", specularColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				octoon::imgui::dragFloat("smoothness", &smoothness, 0.01f, 0.0f, 1.0f);
				octoon::imgui::dragFloat("metalness", &metalness, 0.01f, 0.0f, 1.0f);

				lightDir = octoon::math::normalize(lightDir);

				material_->setLightDir(lightDir);
				material_->setBaseColor(baseColor);
				material_->setSpecularColor(specularColor);
				material_->setAmbientColor(ambientColor);
				material_->setSmoothness(smoothness);
				material_->setMetalness(metalness);

				octoon::imgui::treePop();
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
		auto camera = octoon::GamePrefabs::instance()->createPerspectiveCamera(45.0f);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->addComponent<CubeController>();

		auto model = octoon::GamePrefabs::instance()->createModel("luo.pmx");
		
		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}