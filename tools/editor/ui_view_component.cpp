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
			, isShowedMainMenu_(true)
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

			camera_ = GameObject::create("MainCamera");
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
			this->showMainMenu();
			this->showCameraWindow();
			this->showAssetsWindow();
			this->showHierarchyWindow();
			this->showInspectorWindow();
		}

		void
		UIViewComponent::showMainMenu() noexcept
		{
			if (!isShowedMainMenu_)
				return;

			imgui::push_style_var(imgui::GuiStyleVar::WindowPadding, math::float2(_style.WindowPadding.x * 2, _style.WindowPadding.y));

			if (imgui::begin_main_menu_bar())
			{
				math::float2 size = imgui::get_display_size();
				size.y -= imgui::get_window_height();

				imgui::root_dock(math::float2(0, imgui::get_window_height()), size);
				imgui::push_style_color(imgui::GuiCol::Border, math::float4::Zero);

				if (imgui::begin_menu("File"))
				{
					if (imgui::menu_item("Open", "CTRL+O", false, false)) { /*on click */}
					if (imgui::menu_item("Save", "CTRL+S", false, false)) { /*on click */}
					if (imgui::menu_item("Save As...", "CTRL+SHIFT+S", false, false)) { /*on click */}

					imgui::separator();

					if (imgui::menu_item("Import Model")) { /*on click */}
					if (imgui::menu_item("Export Model")) { /*on click */}

					imgui::separator();

					if (imgui::menu_item("Exit")) { std::exit(0); }

					imgui::end_menu();
				}

				if (imgui::begin_menu("Edit"))
				{
					if (imgui::begin_menu("Language"))
					{
						if (imgui::menu_item("English")) { /*on click() */}
						if (imgui::menu_item("Chinese")) { /*on click() */}

						imgui::end_menu();
					}

					imgui::separator();
					imgui::menu_item("Style Editor", 0);

					imgui::end_menu();
				}

				if (imgui::begin_menu("GameObject"))
				{
					if (imgui::menu_item("Create Game Object")) { /*on click */}
					imgui::end_menu();
				}

				if (imgui::begin_menu("Window"))
				{
					imgui::menu_item("Assert", 0);
					imgui::menu_item("Camera", 0);
					imgui::menu_item("Inspector", 0);
					imgui::menu_item("Light Mass", 0);
					imgui::end_menu();
				}

				if (imgui::begin_menu("Help"))
				{
					imgui::menu_item("About", 0);
					imgui::end_menu();
				}

				imgui::pop_style_color();

				imgui::end_main_menu_bar();
			}

			imgui::pop_style_var();
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
#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
					camera_->getComponent<CameraComponent>()->setupFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y);
#else
					camera_->getComponent<CameraComponent>()->setupFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y, 4);
					camera_->getComponent<CameraComponent>()->setupSwapFramebuffers((std::uint32_t)size.x, (std::uint32_t)size.y);
#endif
					framebufferSizeW_ = (std::uint32_t)size.x;
					framebufferSizeH_ = (std::uint32_t)size.y;
				}

#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
				auto framebuffer = cameraComponent->getFramebuffer();
#else
				auto framebuffer = cameraComponent->getSwapFramebuffer();
#endif
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
				if (imgui::tree_node_ex("Camera", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					std::string name = camera_->getName();
					if (imgui::selectable(name.c_str()))
					{
					}

					imgui::tree_pop();
				}

				if (imgui::tree_node_ex("Object", imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
				{
					std::string name = camera_->getName();
					if (imgui::selectable(name.c_str()))
					{
					}

					imgui::tree_pop();
				}

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