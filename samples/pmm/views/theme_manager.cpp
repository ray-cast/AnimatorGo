#include "theme_manager.h"

namespace MysticLit
{
	OctoonImplementSubClass(ThemeManager, octoon::GameComponent, "ThemeManager")

	ThemeManager::ThemeManager() noexcept
	{
		_styleDefault.AntiAliasedLines = false;
		_styleDefault.ItemSpacing.y = 3;
		_styleDefault.WindowPadding.x = 5;
		_styleDefault.WindowPadding.y = 10;
		_styleDefault.FramePadding.x = 6;
		_styleDefault.ScrollbarSize = 15;
		_styleDefault.IndentSpacing = 25;
		_styleDefault.Colors[octoon::imgui::GuiCol::FrameBgActive] = octoon::math::float4(0.3f, 0.3f, 0.3f, 0.58f);
		_styleDefault.Colors[octoon::imgui::GuiCol::FrameBgHovered] = octoon::math::float4(0.4f, 0.4f, 0.4f, 0.75f);
		_styleDefault.Colors[octoon::imgui::GuiCol::WindowBg] = octoon::math::float4(0.075f, 0.075f, 0.075f, 1.0f);
		_styleDefault.Colors[octoon::imgui::GuiCol::ChildBg] = octoon::math::float4(0.33333f, 0.33333f, 0.33333f, 0.45f);
		_styleDefault.Colors[octoon::imgui::GuiCol::ScrollbarGrab] = octoon::math::float4(0.314f, 0.314f, 0.33333f, 1.0f);
		_styleDefault.Colors[octoon::imgui::GuiCol::Border] = octoon::math::float4(0.0f, 0.0f, 0.0f, 1.0f);
		_style = _styleDefault;

		octoon::imgui::setStyle(_styleDefault);
	}

	ThemeManager::~ThemeManager() noexcept
	{
	}

	void
	ThemeManager::onActivate() noexcept
	{
		this->addComponentDispatch(octoon::GameDispatchType::Gui);
	}

	void
	ThemeManager::onDeactivate() noexcept
	{
		this->removeComponentDispatchs();
	}

	void 
	ThemeManager::onGui() noexcept
	{
	}

	octoon::GameComponentPtr
	ThemeManager::clone() const noexcept
	{
		return std::make_shared<ThemeManager>();
	}
}