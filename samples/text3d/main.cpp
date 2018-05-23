#include <octoon/octoon.h>
#include <octoon/octoon-c.h>

class TextController : public octoon::GameComponent
{
public:
	TextController()
	{
	}

	TextController(const octoon::video::TextMaterialPtr& material)
		: material_(material)
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
						auto paths = octoon::model::makeTextPaths(L"Octoon Studio", { "../../system/fonts/DroidSansFallback.ttf", 24, 16 });
						auto aabb = octoon::model::aabb(paths);

						paths -= aabb.center();
						paths /= aabb.extents();
						paths << octoon::model::transform::twist(x1, y1);

						/*paths << octoon::model::transform::fan(x1, y1);
						paths << octoon::model::transform::highCove(x1, y1);
						paths << octoon::model::transform::lowCove(x1, y1);

						paths << octoon::model::transform::cove(x1, y1);
						paths << octoon::model::transform::bulege(x1, y1);
						paths << octoon::model::transform::bulegeHigh(x1, y1);
						paths << octoon::model::transform::bulegeLow(x1, y1);

						paths << octoon::model::transform::flag(x1, y1);
						paths << octoon::model::transform::wave(x1, y1);
						paths << octoon::model::transform::fish(x1, y1);
						paths << octoon::model::transform::slope(x1, y1);

						paths << octoon::model::transform::fishEye(aabb.size().x / aabb.size().y, x1, y1);
						paths << octoon::model::transform::expandOut(x1, y1);
						paths << octoon::model::transform::expandIn(x1, y1);
						paths << octoon::model::transform::spin(aabb.size().x / aabb.size().y, x1, y1);*/

						paths *= aabb.extents();

						auto text = octoon::model::makeTextContours(paths, 8);

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

		auto camera = octoon::GameObject::create();
		camera->addComponent<octoon::CameraComponent>();
		camera->addComponent<octoon::FirstPersonCameraComponent>();
		camera->getComponent<octoon::CameraComponent>()->setCameraOrder(octoon::video::CameraOrder::Main);
		camera->getComponent<octoon::CameraComponent>()->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
		camera->getComponent<octoon::CameraComponent>()->setCameraType(octoon::video::CameraType::Perspective);
		camera->getComponent<octoon::CameraComponent>()->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
		camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 205));

		auto text = octoon::model::makeTextContours(L"Octoon Studio", { "../../system/fonts/DroidSansFallback.ttf", 24 });
		auto aabb = octoon::model::aabb(text);

		for (auto& it : text)
			*it -= aabb.center();

		auto object = octoon::GameObject::create();
		object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeMesh(text));
		object->addComponent<octoon::MeshRendererComponent>(material);
		object->addComponent<octoon::GuizmoComponent>(camera);
		object->addComponent<TextController>(material);

		::OctoonMainLoop();
	}

	::OctoonTerminate();
	return 0;
}