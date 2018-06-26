#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

class TextController : public octoon::GameComponent
{
public:
	TextController()
	{
	}

	TextController(const octoon::video::TextMaterialPtr& material, const std::wstring& text)
		: material_(material)
		, text_(text)
	{
	}

	void onActivate() override
	{
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
				static octoon::math::float1 lern = 0.0f;
				static octoon::math::float3 frontColor = octoon::math::float3(31.0, 179.0, 249.0) / 255.0f;
				static octoon::math::float3 sideColor(0.0f, 1.0f, 0.0f);
				static octoon::math::float3 translate(0.0f, 0.0f, 0.0f);

				octoon::imgui::drag_float("lern", &lern, 0.01f, 0.0f, 1.0f);
				octoon::imgui::drag_float3("translate", translate.ptr(), 1.0f, 0.0f, 50.0f);

				octoon::imgui::color_picker3("front color", frontColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);
				octoon::imgui::color_picker3("side color", sideColor.ptr(), octoon::imgui::GuiColorEditFlagBits::HSV | octoon::imgui::GuiColorEditFlagBits::NoSidePreview);

				material_->setLean(lern);
				material_->setTextColor(octoon::video::TextColor::FrontColor, frontColor);
				material_->setTextColor(octoon::video::TextColor::SideColor, sideColor);
				material_->setTranslate(translate);

				octoon::imgui::tree_pop();
			}

			if (octoon::imgui::tree_node_ex("Text", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
			{
				static octoon::math::float1 x1 = 0.0f;
				static octoon::math::float1 x2 = 0.0f;
				static octoon::math::float1 y1 = 0.0f;
				static octoon::math::float1 y2 = 0.0f;

				octoon::imgui::drag_float("x", &x1, 0.01f, -1, 1);
				octoon::imgui::drag_float("y", &y1, 0.01f, -1, 1); 

				if (y1 != y2 || x1 != x2)
				{
					auto component = this->getComponent<octoon::MeshFilterComponent>();
					if (component)
					{
						auto path = octoon::model::makeTextPaths(text_, { "../../system/fonts/DroidSansFallback.ttf", 24 });
						path << octoon::model::deform::smoother(4);

						auto text = octoon::model::makeTextContours(path, 8);
						auto aabb = octoon::model::aabb(text);

						text -= aabb.center();
						text /= octoon::math::float3(aabb.extents().xy(), 1.0);
						text << octoon::model::deform::bulegeLow(x1, aabb.size().x / aabb.size().y, true);
						text *= octoon::math::float3(aabb.extents().xy(), 1.0);

						// std::cout << paths[0]->getPaths() << std::endl;

						component->setMesh(octoon::model::makeMesh(text, 0.1f));
					}

					x2 = x1;
					y2 = y1;
				}

				octoon::imgui::tree_pop();
			}

			octoon::imgui::end();
		}
	}

	octoon::GameComponentPtr clone() const noexcept override
	{
		return std::make_shared<TextController>();
	}

private:
	std::wstring text_;
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
		material->setTextColor(octoon::video::TextColor::FrontColor, octoon::math::float3(31.0, 179.0, 249.0) / 255.0f);
		material->setTextColor(octoon::video::TextColor::SideColor, octoon::math::float3(0.0, 1.0, 0.0));

		auto camera = octoon::GamePrefabs::instance()->createCamera(45.0f);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, -205));

		auto str = L"Octoon Stdudio";
		auto text = octoon::model::makeTextContours(str, { "../../system/fonts/DroidSansFallback.ttf", 24 });
		auto aabb = octoon::model::aabb(text);

		for (auto& it : text)
			*it -= aabb.center();

		auto object = octoon::GameObject::create();
		object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeMesh(text));
		object->addComponent<octoon::MeshRendererComponent>(material);
		object->addComponent<octoon::GuizmoComponent>(camera);
		object->addComponent<TextController>(material, str);

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}