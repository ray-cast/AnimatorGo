#include "ui_view_component.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(UIViewComponent, GameComponent, "UIViewComponent")

		UIViewComponent::UIViewComponent() noexcept
			: _isShowedHierarchyWindow(true)
			, _isShowedAssetsWindow(true)
			, _isShowedInspectorWindow(true)
			, _isShowedCameraWindow(true)
			, framebufferSizeW_(0)
			, framebufferSizeH_(0)
		{
		}

		UIViewComponent::~UIViewComponent() noexcept
		{
		}

		void
		UIViewComponent::onActivate() noexcept
		{
			this->addComponentDispatch(octoon::GameDispatchType::Gui);

			_camera = octoon::GameObject::create();
			_camera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 205));

			auto camera = _camera->addComponent<octoon::CameraComponent>();
			camera->setCameraOrder(octoon::video::CameraOrder::Custom);
			camera->setClearColor(octoon::math::float4(0.1f, 0.2f, 0.3f, 1.0));
			camera->setCameraType(octoon::video::CameraType::Perspective);
			camera->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));

			auto text = octoon::model::makeTextContours(L"Octoon Studio", { "../../system/fonts/DroidSansFallback.ttf", 24 });
			auto aabb = octoon::model::aabb(text);

			for (auto& it : text)
				*it -= aabb.center();

			auto material = std::make_shared<octoon::video::TextMaterial>();
			material->setLean(0.0f);
			material->setTextColor(octoon::video::TextColor::FrontColor, octoon::math::float3(31.0, 179.0, 249.0) / 255.0f);
			material->setTextColor(octoon::video::TextColor::SideColor, octoon::math::float3(0.0, 1.0, 0.0));

			static auto object = octoon::GameObject::create();
			object->addComponent<octoon::MeshFilterComponent>(octoon::model::makeMesh(text));
			object->addComponent<octoon::MeshRendererComponent>(material);
		}

		void
		UIViewComponent::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();

			_camera.reset();
		}

		void 
		UIViewComponent::onGui() noexcept
		{
			this->showCameraWindow();
			this->showAssetsWindow();
			this->showHierarchyWindow();
			this->showInspectorWindow();
		}

		void
		UIViewComponent::showCameraWindow() noexcept
		{
			auto cameraComponent = _camera->getComponent<CameraComponent>();

			if (imgui::begin_dock("Camera", &_isShowedCameraWindow, octoon::imgui::GuiWindowFlagBits::AlwaysUseWindowPaddingBit | octoon::imgui::GuiWindowFlagBits::NoScrollWithMouseBit))
			{
				imgui::set_scroll_y(imgui::get_style_default().WindowPadding.y);

				auto size = octoon::imgui::get_window_size();

				if (framebufferSizeW_ != size.x || framebufferSizeH_ != size.y)
				{
					_camera->getComponent<CameraComponent>()->setupFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y, 4);
					_camera->getComponent<CameraComponent>()->setupSwapFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y);
					framebufferSizeW_ = size.x;
					framebufferSizeH_ = size.y;
				}

				auto framebuffer = cameraComponent->getSwapFramebuffer();
				if (framebuffer)
				{
					auto texture = framebuffer->getGraphicsFramebufferDesc().getColorAttachment().getBindingTexture();
					if (texture)
						octoon::imgui::image(texture.get(), size, octoon::math::float2::UnitY, octoon::math::float2::UnitX);
				}

				imgui::end_dock();
			}
		}

		void
		UIViewComponent::showAssetsWindow() noexcept
		{
			if (imgui::begin_dock("Assets", &_isShowedAssetsWindow))
			{
				imgui::end_dock();
			}
		}

		void
		UIViewComponent::showHierarchyWindow() noexcept
		{
			if (imgui::begin_dock("Hierarchy", &_isShowedHierarchyWindow))
			{
				imgui::end_dock();
			}
		}

		void 
		UIViewComponent::showInspectorWindow() noexcept
		{
			if (!_isShowedInspectorWindow)
				return;

			if (imgui::begin_dock("Inspector", &_isShowedInspectorWindow))
			{
				imgui::end_dock();
			}
		}

		GameComponentPtr 
		UIViewComponent::clone() const noexcept
		{
			return std::make_shared<UIViewComponent>();
		}
	}
}