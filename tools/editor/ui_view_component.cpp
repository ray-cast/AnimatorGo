#include "ui_view_component.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(UIViewComponent, GameComponent, "UIViewComponent")

		UIViewComponent::UIViewComponent() noexcept
			: isShowedHierarchyWindow_(true)
			, isShowedAssetsWindow_(true)
			, isShowedInspectorWindow_(true)
			, isShowedCameraWindow_(true)
			, framebufferSizeW_(0)
			, framebufferSizeH_(0)
		{
			_styleDefault.AntiAliasedLines = false;
			_styleDefault.ItemSpacing.y = 3;
			_styleDefault.WindowPadding.x = 5;
			_styleDefault.WindowPadding.y = 10;
			_styleDefault.FramePadding.x = 6;
			_styleDefault.ScrollbarSize = 15;
			_styleDefault.IndentSpacing = 25;
			_styleDefault.Colors[imgui::GuiCol::FrameBgActive] = octoon::math::float4(0.3f, 0.3f, 0.3f, 0.58f);
			_styleDefault.Colors[imgui::GuiCol::FrameBgHovered] = octoon::math::float4(0.4f, 0.4f, 0.4f, 0.75f);
			_styleDefault.Colors[imgui::GuiCol::WindowBg] = octoon::math::float4(0.075f, 0.075f, 0.075f, 1.0f);
			_styleDefault.Colors[imgui::GuiCol::ChildBg] = octoon::math::float4(0.33333f, 0.33333f, 0.33333f, 0.45f);
			_styleDefault.Colors[imgui::GuiCol::ScrollbarGrab] = octoon::math::float4(0.314f, 0.314f, 0.33333f, 1.0f);
			_styleDefault.Colors[imgui::GuiCol::Border] = octoon::math::float4(0.0f, 0.0f, 0.0f, 1.0f);
			_style = _styleDefault;

			imgui::set_style(_styleDefault);
		}

		UIViewComponent::~UIViewComponent() noexcept
		{
		}

		void
		UIViewComponent::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);

			camera_ = GameObject::create();
			camera_->getComponent<TransformComponent>()->setTranslate(math::float3(0, 0, 205));

			auto camera = camera_->addComponent<CameraComponent>();
			camera->setCameraOrder(video::CameraOrder::Custom);
			camera->setClearColor(math::float4(0.1f, 0.2f, 0.3f, 1.0));
			camera->setCameraType(video::CameraType::Perspective);
			camera->setOrtho(math::float4(0.0, 1.0, 0.0, 1.0));

			auto text = model::makeTextContours(L"Octoon Studio", { "../../system/fonts/DroidSansFallback.ttf", 24 });
			auto aabb = model::aabb(text);

			for (auto& it : text)
				*it -= aabb.center();

			auto material = std::make_shared<video::TextMaterial>();
			material->setLean(0.0f);
			material->setTextColor(video::TextColor::FrontColor, math::float3(31.0, 179.0, 249.0) / 255.0f);
			material->setTextColor(video::TextColor::SideColor, math::float3(0.0, 1.0, 0.0));

			static auto object = GameObject::create();
			object->addComponent<MeshFilterComponent>(model::makeMesh(text));
			object->addComponent<MeshRendererComponent>(material);
		}

		void
		UIViewComponent::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();

			camera_.reset();
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
			auto cameraComponent = camera_->getComponent<CameraComponent>();

			if (imgui::begin_dock("Camera", &isShowedCameraWindow_, imgui::GuiWindowFlagBits::AlwaysUseWindowPaddingBit | imgui::GuiWindowFlagBits::NoScrollWithMouseBit))
			{
				imgui::set_scroll_y(imgui::get_style_default().WindowPadding.y);

				auto size = imgui::get_window_size();

				if (framebufferSizeW_ != size.x || framebufferSizeH_ != size.y)
				{
					camera_->getComponent<CameraComponent>()->setupFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y, 4);
					camera_->getComponent<CameraComponent>()->setupSwapFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y);
					framebufferSizeW_ = (std::uint32_t)size.x;
					framebufferSizeH_ = (std::uint32_t)size.y;
				}

				auto framebuffer = cameraComponent->getSwapFramebuffer();
				if (framebuffer)
				{
					auto texture = framebuffer->getGraphicsFramebufferDesc().getColorAttachment().getBindingTexture();
					if (texture)
						imgui::image(texture.get(), size, math::float2::UnitY, math::float2::UnitX);
				}

				imgui::end_dock();
			}
		}

		void
		UIViewComponent::showAssetsWindow() noexcept
		{
			if (imgui::begin_dock("Assets", &isShowedAssetsWindow_))
			{
				imgui::end_dock();
			}
		}

		void
		UIViewComponent::showHierarchyWindow() noexcept
		{
			if (imgui::begin_dock("Hierarchy", &isShowedHierarchyWindow_))
			{
				imgui::end_dock();
			}
		}

		void 
		UIViewComponent::showInspectorWindow() noexcept
		{
			if (!isShowedInspectorWindow_)
				return;

			if (imgui::begin_dock("Inspector", &isShowedInspectorWindow_))
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