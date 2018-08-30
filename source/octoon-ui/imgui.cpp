#include <octoon/ui/imgui.h>

#include <imgui.h>
#include <imgui_dock.h>
#include <imgui_internal.h>
#include <imgui_user.h>
#include <ImGuizmo.h>

#include <cstring> // std::memcpy

namespace octoon
{
	namespace imgui
	{
		GuiStyle _defalutStyle;

		float2 ImVec2ToFloat2(ImVec2 vec2)
		{
			return float2(vec2.x, vec2.y);
		}

		float4 ImVec4ToFloat4(ImVec4 vec4)
		{
			return float4(vec4.x, vec4.y, vec4.z, vec4.w);
		}

		GuiStyle::GuiStyle() noexcept
		{
			Alpha = 1.0f;
			WindowPadding = float2(8, 8);
			WindowMinSize = float2(32, 32);
			WindowRounding = 9.0f;
			WindowTitleAlign = float2(0.5f, 0.5f);
			ChildRounding = 0.0f;
			ChildBorderSize = 0.0f;
			FramePadding = float2(3, 3);
			FrameRounding = 8.0f;
			ItemSpacing = float2(8, 4);
			ItemInnerSpacing = float2(4, 4);
			TouchExtraPadding = float2(0, 0);
			IndentSpacing = 15.0f;
			ColumnsMinSpacing = 6.0f;
			ScrollbarSize = 16.0f;
			ScrollbarRounding = 9.0f;
			GrabMinSize = 10.0f;
			GrabRounding = 5.0f;
			ButtonTextAlign = float2(0.5f, 0.5f);
			DisplayWindowPadding = float2(9, 22);
			DisplaySafeAreaPadding = float2(4, 4);
			AntiAliasedLines = true;
			AntiAliasedFill = true;
			CurveTessellationTol = 1.25f;

			Colors[ImGuiCol_MenuBarBg] = float4(0.3f, 0.3f, 0.31f, 1.0f);
			Colors[ImGuiCol_Text] = float4(1.0f, 1.0f, 1.0f, 1.0f);
			Colors[ImGuiCol_TextDisabled] = float4(0.58823f, 0.58823f, 0.58823f, 0.58823f);
			Colors[ImGuiCol_WindowBg] = float4(0.0f, 0.0, 0.0, 0.8627f);
			Colors[ImGuiCol_Header] = float4(0.13333f, 0.52549f, 0.9019f, 0.70196f);
			Colors[ImGuiCol_HeaderHovered] = float4(0.13333f, 0.52549f, 0.9019f, 0.70196f);
			Colors[ImGuiCol_HeaderActive] = float4(0.09411f, 0.37647f, 0.784313f, 0.70196f);
			Colors[ImGuiCol_TitleBg] = float4(0.0f, 0.0, 0.0, 0.70196f);
			Colors[ImGuiCol_TitleBgCollapsed] = float4(0.0f, 0.0, 0.0, 0.70196f);
			Colors[ImGuiCol_TitleBgActive] = float4(0.0f, 0.0, 0.0, 0.70196f);
			Colors[ImGuiCol_Button] = float4(0.0f, 0.4f, 0.66f, 1.0f);
			Colors[ImGuiCol_ButtonHovered] = float4(0.0f, 0.5f, 1.0f, 1.0f);
			Colors[ImGuiCol_ButtonActive] = float4(1.0, 0.6, 0.0, 1.0);
			Colors[ImGuiCol_CheckMark] = float4(0.0, 0.6, 1.0, 0.75f);
			Colors[ImGuiCol_ScrollbarBg] = float4(0.20f, 0.20, 0.20f, 0.60f);
			Colors[ImGuiCol_ScrollbarGrab] = float4(0.0f, 0.0f, 0.0f, 1.0f);
			Colors[ImGuiCol_ScrollbarGrabHovered] = float4(0.4901f, 0.4901f, 0.4901f, 0.3922f);
			Colors[ImGuiCol_ScrollbarGrabActive] = float4(0.29411f, 0.29411f, 0.29411f, 0.3922f);
			Colors[ImGuiCol_SliderGrab] = float4(1.00f, 0.7f, 0.0f, 1.00f);
			Colors[ImGuiCol_SliderGrabActive] = float4(1.00f, 0.4f, 0.0f, 1.00f);
			Colors[ImGuiCol_FrameBg] = float4(0.80f, 0.80f, 0.80f, 0.10f);
			Colors[ImGuiCol_FrameBgHovered] = float4(0.90f, 0.80f, 0.80f, 0.20f);
			Colors[ImGuiCol_FrameBgActive] = float4(0.9, 0.5f, 0.0f, 1.0f);

			Colors[ImGuiCol_PopupBg] = float4(0.05f, 0.05f, 0.05f, 0.90f);
			Colors[ImGuiCol_ModalWindowDarkening] = float4(0.10f, 0.10f, 0.10f, 0.5f);

			Colors[ImGuiCol_ChildWindowBg] = float4(0.00f, 0.00f, 0.00f, 0.00f);
			Colors[ImGuiCol_Border] = float4(0.70f, 0.70f, 0.70f, 0.125f);
			Colors[ImGuiCol_BorderShadow] = float4(0.00f, 0.00f, 0.00f, 0.00f);
			Colors[ImGuiCol_Column] = float4(0.50f, 0.50f, 0.50f, 1.00f);
			Colors[ImGuiCol_ColumnHovered] = float4(0.70f, 0.60f, 0.60f, 1.00f);
			Colors[ImGuiCol_ColumnActive] = float4(0.90f, 0.70f, 0.70f, 1.00f);
			Colors[ImGuiCol_ResizeGrip] = float4(1.00f, 1.00f, 1.00f, 0.30f);
			Colors[ImGuiCol_ResizeGripHovered] = float4(1.00f, 1.00f, 1.00f, 0.60f);
			Colors[ImGuiCol_ResizeGripActive] = float4(1.00f, 1.00f, 1.00f, 0.90f);
			Colors[ImGuiCol_PlotLines] = float4(1.00f, 1.00f, 1.00f, 1.00f);
			Colors[ImGuiCol_PlotLinesHovered] = float4(0.90f, 0.70f, 0.00f, 1.00f);
			Colors[ImGuiCol_PlotHistogram] = float4(0.90f, 0.70f, 0.00f, 1.00f);
			Colors[ImGuiCol_PlotHistogramHovered] = float4(1.00f, 0.60f, 0.00f, 1.00f);
			Colors[ImGuiCol_TextSelectedBg] = float4(0.7f, 0.7f, 0.7f, 0.35f);
		}

		void newFrame() noexcept
		{
			ImGui::NewFrame();
		}

		void showUserGuide() noexcept
		{
			ImGui::ShowUserGuide();
		}

		void showStyleEditor(GuiStyle* ref) noexcept
		{
			static_assert(sizeof(GuiStyle) == sizeof(ImGuiStyle), "");
			static_assert(offsetof(GuiStyle, Alpha) == offsetof(ImGuiStyle, Alpha), "");
			static_assert(offsetof(GuiStyle, WindowPadding) == offsetof(ImGuiStyle, WindowPadding), "");
			static_assert(offsetof(GuiStyle, WindowRounding) == offsetof(ImGuiStyle, WindowRounding), "");
			static_assert(offsetof(GuiStyle, WindowBorderSize) == offsetof(ImGuiStyle, WindowBorderSize), "");
			static_assert(offsetof(GuiStyle, WindowMinSize) == offsetof(ImGuiStyle, WindowMinSize), "");
			static_assert(offsetof(GuiStyle, WindowTitleAlign) == offsetof(ImGuiStyle, WindowTitleAlign), "");
			static_assert(offsetof(GuiStyle, ChildRounding) == offsetof(ImGuiStyle, ChildRounding), "");
			static_assert(offsetof(GuiStyle, ChildBorderSize) == offsetof(ImGuiStyle, ChildBorderSize), "");
			static_assert(offsetof(GuiStyle, FramePadding) == offsetof(ImGuiStyle, FramePadding), "");
			static_assert(offsetof(GuiStyle, FrameRounding) == offsetof(ImGuiStyle, FrameRounding), "");
			static_assert(offsetof(GuiStyle, ItemSpacing) == offsetof(ImGuiStyle, ItemSpacing), "");
			static_assert(offsetof(GuiStyle, ItemInnerSpacing) == offsetof(ImGuiStyle, ItemInnerSpacing), "");
			static_assert(offsetof(GuiStyle, TouchExtraPadding) == offsetof(ImGuiStyle, TouchExtraPadding), "");
			static_assert(offsetof(GuiStyle, IndentSpacing) == offsetof(ImGuiStyle, IndentSpacing), "");
			static_assert(offsetof(GuiStyle, ColumnsMinSpacing) == offsetof(ImGuiStyle, ColumnsMinSpacing), "");
			static_assert(offsetof(GuiStyle, ScrollbarSize) == offsetof(ImGuiStyle, ScrollbarSize), "");
			static_assert(offsetof(GuiStyle, ScrollbarRounding) == offsetof(ImGuiStyle, ScrollbarRounding), "");
			static_assert(offsetof(GuiStyle, GrabMinSize) == offsetof(ImGuiStyle, GrabMinSize), "");
			static_assert(offsetof(GuiStyle, GrabRounding) == offsetof(ImGuiStyle, GrabRounding), "");
			static_assert(offsetof(GuiStyle, ButtonTextAlign) == offsetof(ImGuiStyle, ButtonTextAlign), "");
			static_assert(offsetof(GuiStyle, DisplayWindowPadding) == offsetof(ImGuiStyle, DisplayWindowPadding), "");
			static_assert(offsetof(GuiStyle, DisplaySafeAreaPadding) == offsetof(ImGuiStyle, DisplaySafeAreaPadding), "");
			static_assert(offsetof(GuiStyle, AntiAliasedLines) == offsetof(ImGuiStyle, AntiAliasedLines), "");
			static_assert(offsetof(GuiStyle, AntiAliasedFill) == offsetof(ImGuiStyle, AntiAliasedFill), "");
			static_assert(offsetof(GuiStyle, CurveTessellationTol) == offsetof(ImGuiStyle, CurveTessellationTol), "");

			if (ref)
				ImGui::ShowStyleEditor((ImGuiStyle*)ref);
			else
				ImGui::ShowStyleEditor((ImGuiStyle*)&_defalutStyle);
		}

		void showTestWindow() noexcept
		{
			ImGui::ShowTestWindow();
		}

		void showMetricsWindow(bool* isOpened) noexcept
		{
			if (!isOpened)
				ImGui::ShowMetricsWindow();
			else
				ImGui::ShowMetricsWindow(isOpened);
		}

		bool begin(const char* name, bool* isOpened, GuiWindowFlags flags) noexcept
		{
			if (!ImGui::Begin(name, isOpened, flags))
			{
				ImGui::End();
				return false;
			}

			return true;
		}

		bool begin(const char* name, bool* isOpened, const float2& size_on_first_use, float alpha, GuiWindowFlags flags) noexcept
		{
			if (!ImGui::Begin(name, isOpened, (const ImVec2&)size_on_first_use, alpha, flags))
			{
				ImGui::End();
				return false;
			}

			return true;
		}

		void end() noexcept
		{
			ImGui::End();
		}

		bool beginChild(const char* str_id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChild(str_id, (ImVec2&)size, border, extraFlags))
			{
				ImGui::EndChild();
				return false;
			}

			return true;
		}

		bool beginChild(GuiID id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChild((ImGuiID)id, (ImVec2&)size, border, extraFlags))
			{
				ImGui::EndChild();
				return false;
			}

			return true;
		}

		bool beginChildFrame(GuiID id, const float2& size, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChildFrame(id, (const ImVec2&)size, extraFlags))
			{
				ImGui::EndChildFrame();
				return false;
			}

			return true;
		}

		void endChild() noexcept
		{
			return ImGui::EndChild();
		}

		void endChildFrame() noexcept
		{
			return ImGui::EndChildFrame();
		}

		float2 getContentRegionMax() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetContentRegionMax());
		}

		float2 getContentRegionAvail() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetContentRegionAvail());
		}

		float getContentRegionAvailWidth() noexcept
		{
			return ImGui::GetContentRegionAvailWidth();
		}

		float2 getWindowContentRegionMin() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowContentRegionMin());
		}

		float2 getWindowContentRegionMax() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowContentRegionMax());
		}

		float getWindowContentRegionWidth() noexcept
		{
			return ImGui::GetWindowContentRegionWidth();
		}

		float2 getWindowPos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowPos());
		}

		float2 getWindowSize() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowSize());
		}

		float getWindowWidth() noexcept
		{
			return ImGui::GetWindowWidth();
		}

		float getWindowHeight() noexcept
		{
			return ImGui::GetWindowHeight();
		}

		bool isWindowCollapsed() noexcept
		{
			return ImGui::IsWindowCollapsed();
		}

		void setWindowFontScale(float scale) noexcept
		{
			ImGui::SetWindowFontScale(scale);
		}

		void setNextWindowPos(const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowPos((const ImVec2&)pos, cond);
		}

		void setNextWindowPosCenter(GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowPosCenter(cond);
		}

		void setNextWindowSize(const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowSize((const ImVec2&)size);
		}

		void setNextWindowSizeConstraints(const float2& min, const float2& max) noexcept
		{
			ImGui::SetNextWindowSizeConstraints((const ImVec2&)min, (const ImVec2&)max);
		}

		void setNextWindowContentSize(const float2& size) noexcept
		{
			ImGui::SetNextWindowContentSize((const ImVec2&)size);
		}

		void setNextWindowContentWidth(float width) noexcept
		{
			ImGui::SetNextWindowContentWidth(width);
		}

		void setNextWindowCollapsed(bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowCollapsed(collapsed, cond);
		}

		void setNextWindowFocus() noexcept
		{
			ImGui::SetNextWindowFocus();
		}

		void setWindowPos(const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowPos((const ImVec2&)pos, cond);
		}

		void setWindowSize(const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowSize((const ImVec2&)size, cond);
		}

		void setWindowCollapsed(bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowCollapsed(collapsed, cond);
		}

		void setWindowFocus() noexcept
		{
			ImGui::SetWindowFocus();
		}

		void setWindowPos(const char* name, const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowPos(name, (const ImVec2&)pos, cond);
		}

		void setWindowSize(const char* name, const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowSize(name, (const ImVec2&)size, cond);
		}

		void setWindowCollapsed(const char* name, bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowCollapsed(name, collapsed, cond);
		}

		void setWindowFocus(const char* name) noexcept
		{
			ImGui::SetWindowFocus();
		}

		float getScrollX() noexcept
		{
			return ImGui::GetScrollX();
		}

		float getScrollY() noexcept
		{
			return ImGui::GetScrollY();
		}

		float getScrollMaxX() noexcept
		{
			return ImGui::GetScrollMaxX();
		}

		float getScrollMaxY() noexcept
		{
			return ImGui::GetScrollMaxY();
		}

		void setScrollX(float scroll_x) noexcept
		{
			ImGui::SetScrollX(scroll_x);
		}

		void setScrollY(float scroll_y) noexcept
		{
			ImGui::SetScrollY(scroll_y);
		}

		void setScrollHere(float center_y_ratio) noexcept
		{
			ImGui::SetScrollHere(center_y_ratio);
		}

		void setScrollFromPosY(float pos_y, float center_y_ratio) noexcept
		{
			ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
		}

		void setKeyboardFocusHere(int offset) noexcept
		{
			ImGui::SetKeyboardFocusHere(offset);
		}

		void pushStyleColor(GuiCol::Type idx, const float4& col) noexcept
		{
			ImGui::PushStyleColor((ImGuiCol)idx, (ImVec4&)col);
		}

		void popStyleColor(int count) noexcept
		{
			ImGui::PopStyleColor(count);
		}

		void pushStyleVar(GuiStyleVar idx, float val) noexcept
		{
			ImGui::PushStyleVar((ImGuiStyleVar)idx, val);
		}

		void pushStyleVar(GuiStyleVar idx, const float2& val) noexcept
		{
			ImGui::PushStyleVar((ImGuiStyleVar)idx, (ImVec2&)val);
		}

		void popStyleVar(int count) noexcept
		{
			ImGui::PopStyleVar(count);
		}

		float getFontSize() noexcept
		{
			return ImGui::GetFontSize();
		}

		float2 getFontTexUvWhitePixel() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetFontTexUvWhitePixel());
		}

		std::uint32_t getColorU32(GuiCol::Type idx, float alpha_mul) noexcept
		{
			return ImGui::GetColorU32((ImGuiCol)idx, alpha_mul);
		}

		std::uint32_t getColorU32(const float4& col) noexcept
		{
			return ImGui::GetColorU32((ImVec4&)col);
		}

		void pushItemWidth(float item_width) noexcept
		{
			ImGui::PushItemWidth(item_width);
		}

		void popItemWidth() noexcept
		{
			ImGui::PopItemWidth();
		}

		float calcItemWidth() noexcept
		{
			return ImGui::CalcItemWidth();
		}

		void pushTextWrapPos(float wrap_pos_x) noexcept
		{
			ImGui::PushTextWrapPos();
		}

		void popTextWrapPos() noexcept
		{
			ImGui::PopTextWrapPos();
		}

		void pushAllowKeyboardFocus(bool v) noexcept
		{
			ImGui::PushAllowKeyboardFocus(v);
		}

		void popAllowKeyboardFocus() noexcept
		{
			ImGui::PopAllowKeyboardFocus();
		}

		void pushButtonRepeat(bool repeat) noexcept
		{
			ImGui::PushButtonRepeat(repeat);
		}

		void popButtonRepeat() noexcept
		{
			ImGui::PopButtonRepeat();
		}

		void separator() noexcept
		{
			ImGui::Separator();
		}

		void sameLine(float pos_x, float spacing_w) noexcept
		{
			ImGui::SameLine(pos_x, spacing_w);
		}

		void newLine() noexcept
		{
			ImGui::NewLine();
		}

		void spacing() noexcept
		{
			ImGui::Spacing();
		}

		void dummy(const float2& size) noexcept
		{
			ImGui::Dummy((ImVec2&)size);
		}

		void indent(float indent_w) noexcept
		{
			ImGui::Indent(indent_w);
		}

		void unindent(float indent_w) noexcept
		{
			ImGui::Unindent(indent_w);
		}

		void beginGroup() noexcept
		{
			ImGui::BeginGroup();
		}

		void endGroup() noexcept
		{
			ImGui::EndGroup();
		}

		float2 getCursorPos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorPos());
		}

		float getCursorPosX() noexcept
		{
			return ImGui::GetCursorPosX();
		}

		float getCursorPosY() noexcept
		{
			return ImGui::GetCursorPosY();
		}

		float2 getCursorStartPos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorStartPos());
		}

		float2 getCursorScreenPos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorScreenPos());
		}

		void setCursorPos(const float2& local_pos) noexcept
		{
			ImGui::SetCursorPos((ImVec2&)local_pos);
		}

		void setCursorPosX(float x) noexcept
		{
			ImGui::SetCursorPosX(x);
		}

		void setCursorPosY(float y) noexcept
		{
			ImGui::SetCursorPosY(y);
		}

		void setCursorScreenPos(const float2& pos) noexcept
		{
			return ImGui::SetCursorScreenPos((ImVec2&)pos);
		}

		void alignFirstTextHeightToWidgets() noexcept
		{
			ImGui::AlignFirstTextHeightToWidgets();
		}

		float getTextLineHeight() noexcept
		{
			return ImGui::GetTextLineHeight();
		}

		float getTextLineHeightWithSpacing() noexcept
		{
			return ImGui::GetTextLineHeightWithSpacing();
		}

		float getItemsLineHeightWithSpacing() noexcept
		{
			return ImGui::GetItemsLineHeightWithSpacing();
		}

		void columns(int count, const char* id, bool border) noexcept
		{
			ImGui::Columns(count, id, border);
		}

		void pushID(const char* str_id_begin, const char* str_id_end) noexcept
		{
			ImGui::PushID(str_id_begin, str_id_end);
		}

		void pushID(const void* ptr_id) noexcept
		{
			ImGui::PushID(ptr_id);
		}

		void pushID(int int_id) noexcept
		{
			ImGui::PushID(int_id);
		}

		void popID() noexcept
		{
			ImGui::PopID();
		}

		GuiID getID(const char* str_id) noexcept
		{
			return ImGui::GetID(str_id);
		}

		GuiID getID(const char* str_id_begin, const char* str_id_end) noexcept
		{
			return ImGui::GetID(str_id_begin, str_id_end);
		}

		GuiID getID(const void* ptr_id) noexcept
		{
			return ImGui::GetID(ptr_id);
		}

		void text(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextV(fmt, args);
			va_end(args);
		}

		void textV(const char* fmt, va_list args) noexcept
		{
			ImGui::TextV(fmt, args);
		}

		void textColored(const float4& col, const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextColoredV((ImVec4&)col, fmt, args);
			va_end(args);
		}

		void textColoredV(const float4& col, const char* fmt, va_list args) noexcept
		{
			ImGui::TextColoredV((ImVec4&)col, fmt, args);
		}

		void textDisabled(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextDisabledV(fmt, args);
			va_end(args);
		}

		void textDisabledV(const char* fmt, va_list args) noexcept
		{
			ImGui::TextDisabledV(fmt, args);
		}

		void textWrapped(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextWrappedV(fmt, args);
			va_end(args);
		}

		void textWrappedV(const char* fmt, va_list args) noexcept
		{
			ImGui::TextWrappedV(fmt, args);
		}

		void textUnformatted(const char* text, const char* text_end) noexcept
		{
			ImGui::TextUnformatted(text, text_end);
		}

		void labelText(const char* label, const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::LabelTextV(label, fmt, args);
			va_end(args);
		}

		void labelTextV(const char* label, const char* fmt, va_list args) noexcept
		{
			ImGui::LabelTextV(label, fmt, args);
		}

		void bullet() noexcept
		{
			ImGui::Bullet();
		}

		void bulletText(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::BulletTextV(fmt, args);
			va_end(args);
		}

		void bulletTextV(const char* fmt, va_list args) noexcept
		{
			ImGui::BulletTextV(fmt, args);
		}

		bool button(const char* label, const float2& size) noexcept
		{
			return ImGui::Button(label, (ImVec2&)size);
		}

		bool smallButton(const char* label) noexcept
		{
			return ImGui::SmallButton(label);
		}

		bool invisibleButton(const char* str_id, const float2& size) noexcept
		{
			return ImGui::InvisibleButton(str_id, (ImVec2&)size);
		}

		bool arrowButton(const char* str_id, GuiDir dir)
		{
			return ImGui::ArrowButton(str_id, (ImGuiDir)dir);
		}

		void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, const float4& tint_col, const float4& border_col) noexcept
		{
			ImGui::Image(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, (ImVec4&)tint_col, (ImVec4&)border_col);
		}

		bool imageButton(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col) noexcept
		{
			return ImGui::ImageButton(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, frame_padding, (ImVec4&)bg_col, (ImVec4&)tint_col);
		}

		bool checkbox(const char* label, bool* v) noexcept
		{
			return ImGui::Checkbox(label, v);
		}

		bool checkboxFlags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept
		{
			return ImGui::CheckboxFlags(label, flags, flags_value);
		}

		bool radioButton(const char* label, bool active) noexcept
		{
			return ImGui::RadioButton(label, active);
		}

		bool radioButton(const char* label, int* v, int v_button) noexcept
		{
			return ImGui::RadioButton(label, v, v_button);
		}

		bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
		{
			return ImGui::Combo(label, current_item, items, items_count, height_in_items);
		}

		bool comboWithRevert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items) noexcept
		{
			bool change = ImGui::Combo(label, current_item, items, items_count, height_in_items);
			if (*current_item != _default)
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(revert)) { *current_item = _default; change = false; };
				popID();
			}

			return change;
		}

		bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items) noexcept
		{
			return ImGui::Combo(label, current_item, items_separated_by_zeros, height_in_items);
		}

		bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items) noexcept
		{
			return ImGui::Combo(label, current_item, items_getter, data, items_count, height_in_items);
		}

		bool colorButton(const char* desc_id, const float4& col, GuiColorEditFlags flags, float2 size) noexcept
		{
			return ImGui::ColorButton(desc_id, (ImVec4&)col, flags, ImVec2(size.x, size.y));
		}

		bool colorEdit3(const char* label, float col[3]) noexcept
		{
			return ImGui::ColorEdit3(label, col);
		}

		bool colorEdit4(const char* label, float col[4], bool show_alpha) noexcept
		{
			return ImGui::ColorEdit4(label, col, show_alpha);
		}

		void plotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
		{
			ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
		}

		void plotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
		{
			ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size);
		}

		void plotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
		{
			ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
		}

		void plotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
		{
			ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max);
		}

		void progressBar(float fraction, const float2& size_arg, const char* overlay) noexcept
		{
			ImGui::ProgressBar(fraction, (ImVec2&)size_arg, overlay);
		}

		bool dragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool dragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool dragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool dragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool dragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power) noexcept
		{
			return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
		}

		bool dragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt(label, v, v_speed, v_min, v_max, display_format);
		}

		bool dragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt2(label, v, v_speed, v_min, v_max, display_format);
		}

		bool dragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt3(label, v, v_speed, v_min, v_max, display_format);
		}

		bool dragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt4(label, v, v_speed, v_min, v_max, display_format);
		}

		bool dragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max) noexcept
		{
			return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format);
		}

		bool inputText(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags) noexcept
		{
			return ImGui::InputText(label, buf, buf_size, flags);
		}

		bool inputTextMultiline(const char* label, char* buf, size_t buf_size, const float2& size, GuiInputTextFlags flags) noexcept
		{
			return ImGui::InputTextMultiline(label, buf, buf_size, (const ImVec2&)size, flags);
		}

		bool inputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extraFlags);
		}

		bool inputFloat2(const char* label, float v[2], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat2(label, v, decimal_precision, extraFlags);
		}

		bool inputFloat3(const char* label, float v[3], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat3(label, v, decimal_precision, extraFlags);
		}

		bool inputFloat4(const char* label, float v[4], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat4(label, v, decimal_precision, extraFlags);
		}

		bool inputInt(const char* label, int* v, int step, int step_fast, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt(label, v, step, step_fast, extraFlags);
		}

		bool inputInt2(const char* label, int v[2], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt2(label, v, extraFlags);
		}

		bool inputInt3(const char* label, int v[3], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt3(label, v, extraFlags);
		}

		bool inputInt4(const char* label, int v[4], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt4(label, v, extraFlags);
		}

		bool sliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat(label, v, v_min, v_max, display_format, power);
		}

		bool sliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat2(label, v, v_min, v_max, display_format, power);
		}

		bool sliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat3(label, v, v_min, v_max, display_format, power);
		}

		bool sliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat4(label, v, v_min, v_max, display_format, power);
		}

		bool sliderFloatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::VSliderFloat(label, (ImVec2&)size, v, v_min, v_max, display_format, power);
		}

		bool sliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max) noexcept
		{
			return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
		}

		bool sliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt(label, v, v_min, v_max, display_format);
		}

		bool sliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt2(label, v, v_min, v_max, display_format);
		}

		bool sliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt3(label, v, v_min, v_max, display_format);
		}

		bool sliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt4(label, v, v_min, v_max, display_format);
		}

		bool sliderIntv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::VSliderInt(label, (ImVec2&)size, v, v_min, v_max, display_format);
		}

		bool treeNode(const char* label) noexcept
		{
			return ImGui::TreeNode(label);
		}

		bool treeNode(const char* str_id, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeExV(str_id, 0, fmt, args);
			va_end(args);
			return result;
		}

		bool treeNode(const void* ptr_id, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
			va_end(args);
			return result;
		}

		bool treeNodeV(const char* str_id, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeExV(str_id, 0, fmt, args);
		}

		bool treeNodeV(const void* ptr_id, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
		}

		bool treeNodeEx(const char* label, GuiTreeNodeFlags flags) noexcept
		{
			return ImGui::TreeNodeEx(label, flags);
		}

		bool treeNodeEx(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeEx(str_id, flags, fmt, args);
			va_end(args);
			return result;
		}

		bool treeNodeEx(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
			va_end(args);
			return result;
		}

		bool treeNodeExV(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeEx(str_id, flags, fmt, args);
		}

		bool treeNodeExV(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
		}

		void treePush(const char* str_id) noexcept
		{
			return ImGui::TreePush(str_id);
		}

		void treePush(const void* ptr_id) noexcept
		{
			return ImGui::TreePush(ptr_id);
		}

		void treePop() noexcept
		{
			return ImGui::TreePop();
		}

		void treeAdvanceToLabelPos() noexcept
		{
			return ImGui::TreeAdvanceToLabelPos();
		}

		float getTreeNodeToLabelSpacing() noexcept
		{
			return ImGui::GetTreeNodeToLabelSpacing();
		}

		void setNextTreeNodeOpen(bool is_open, GuiSetCondFlags cond) noexcept
		{
			return ImGui::SetNextTreeNodeOpen(is_open, cond);
		}

		bool collapsingHeader(const char* label, GuiTreeNodeFlags flags) noexcept
		{
			return ImGui::CollapsingHeader(label, flags);
		}

		bool collapsingHeader(const char* label, bool* isOpened, GuiTreeNodeFlags flags) noexcept
		{
			return ImGui::CollapsingHeader(label, isOpened, flags);
		}

		bool selectable(const char* label, bool selected, GuiSelectableFlags flags, const float2& size) noexcept
		{
			return ImGui::Selectable(label, selected, flags, (const ImVec2&)size);
		}

		bool selectable(const char* label, bool* p_selected, GuiSelectableFlags flags, const float2& size) noexcept
		{
			return ImGui::Selectable(label, p_selected, flags, (const ImVec2&)size);
		}

		bool listBox(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
		}

		bool listBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items);
		}

		bool listBoxHeader(const char* label, const float2& size) noexcept
		{
			return ImGui::ListBoxHeader(label, (const ImVec2&)size);
		}

		bool listBoxHeader(const char* label, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBoxHeader(label, items_count, height_in_items);
		}

		void listBoxFooter() noexcept
		{
			ImGui::ListBoxFooter();
		}

		void value(const char* prefix, bool b) noexcept
		{
			ImGui::Value(prefix, b);
		}

		void value(const char* prefix, int v) noexcept
		{
			ImGui::Value(prefix, v);
		}

		void value(const char* prefix, unsigned int v) noexcept
		{
			ImGui::Value(prefix, v);
		}

		void value(const char* prefix, float v, const char* float_format) noexcept
		{
			ImGui::Value(prefix, v, float_format);
		}

		void setTooltip(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::SetTooltipV(fmt, args);
			va_end(args);
		}

		void setTooltipV(const char* fmt, va_list args) noexcept
		{
			ImGui::SetTooltipV(fmt, args);
		}

		void beginTooltip() noexcept
		{
			ImGui::BeginTooltip();
		}

		void endTooltip() noexcept
		{
			ImGui::EndTooltip();
		}

		bool beginMainMenuBar() noexcept
		{
			if (!ImGui::BeginMainMenuBar())
			{
				ImGui::EndMainMenuBar();
				return false;
			}

			return true;
		}

		void endMainMenuBar() noexcept
		{
			ImGui::EndMainMenuBar();
		}

		bool beginMenuBar() noexcept
		{
			return ImGui::BeginMenuBar();
		}

		void endMenuBar() noexcept
		{
			return ImGui::EndMenuBar();
		}

		bool beginMenu(const char* label, bool enabled) noexcept
		{
			return ImGui::BeginMenu(label, enabled);
		}

		void endMenu() noexcept
		{
			return ImGui::EndMenu();
		}

		bool menuItem(const char* label, const char* shortcut, bool selected, bool enabled) noexcept
		{
			return ImGui::MenuItem(label, shortcut, selected, enabled);
		}

		bool menuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled) noexcept
		{
			return ImGui::MenuItem(label, shortcut, p_selected, enabled);
		}

		bool beginPopup(const char* str_id) noexcept
		{
			return ImGui::BeginPopup(str_id);
		}

		bool beginPopupModal(const char* name, bool* isOpened, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginPopupModal(name, isOpened, extraFlags))
			{
				return *isOpened = false;
			}

			return true;
		}

		bool beginPopupContextItem(const char* str_id, int mouse_button) noexcept
		{
			return ImGui::BeginPopupContextItem(str_id, mouse_button);
		}

		bool beginPopupContextWindow(const char* str_id, int mouse_button, bool also_over_items) noexcept
		{
			return ImGui::BeginPopupContextWindow(str_id, mouse_button, also_over_items);
		}

		bool beginPopupContextVoid(const char* str_id, int mouse_button) noexcept
		{
			return ImGui::BeginPopupContextVoid(str_id, mouse_button);
		}

		void openPopup(const char* str_id) noexcept
		{
			return ImGui::OpenPopup(str_id);
		}

		void closeCurrentPopup() noexcept
		{
			return ImGui::CloseCurrentPopup();
		}

		void endPopup() noexcept
		{
			return ImGui::EndPopup();
		}

		void pushClipRect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept
		{
			return ImGui::PushClipRect((const ImVec2&)clip_rect_min, (const ImVec2&)clip_rect_max, intersect_with_current_clip_rect);
		}

		void popClipRect() noexcept
		{
			return ImGui::PopClipRect();
		}

		bool isItemHovered() noexcept
		{
			return ImGui::IsItemHovered();
		}

		bool isItemHoveredRect() noexcept
		{
			return ImGui::IsItemHoveredRect();
		}

		bool isItemActive() noexcept
		{
			return ImGui::IsItemActive();
		}

		bool isItemClicked(int mouse_button) noexcept
		{
			return ImGui::IsItemClicked();
		}

		bool isItemVisible() noexcept
		{
			return ImGui::IsItemVisible();
		}

		bool isAnyItemHovered() noexcept
		{
			return ImGui::IsAnyItemHovered();
		}

		bool isAnyItemActive() noexcept
		{
			return ImGui::IsAnyItemActive();
		}

		float2 getItemRectMin() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectMin());
		}

		float2 getItemRectMax() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectMax());
		}

		float2 getItemRectSize() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectSize());
		}

		void setItemAllowOverlap() noexcept
		{
			return ImGui::SetItemAllowOverlap();
		}

		bool isWindowHovered() noexcept
		{
			return ImGui::IsWindowHovered();
		}

		bool isWindowFocused() noexcept
		{
			return ImGui::IsWindowFocused();
		}

		bool isRootWindowFocused() noexcept
		{
			return ImGui::IsRootWindowFocused();
		}

		bool isRootWindowOrAnyChildFocused() noexcept
		{
			return ImGui::IsRootWindowOrAnyChildFocused();
		}

		bool isRootWindowOrAnyChildHovered() noexcept
		{
			return ImGui::IsRootWindowOrAnyChildHovered();
		}

		bool isRectVisible(const float2& size) noexcept
		{
			return ImGui::IsRectVisible((const ImVec2&)size);
		}

		bool isPosHoveringAnyWindow(const float2& pos) noexcept
		{
			return ImGui::IsPosHoveringAnyWindow((const ImVec2&)pos);
		}

		float getTime() noexcept
		{
			return ImGui::GetTime();
		}

		int getFrameCount() noexcept
		{
			return ImGui::GetFrameCount();
		}

		const char* getStyleColorName(GuiCol::Type idx) noexcept
		{
			return ImGui::GetStyleColorName((ImGuiCol)idx);
		}

		float2 calcItemRectClosestPoint(const float2& pos, bool on_edge, float outward) noexcept
		{
			return ImVec2ToFloat2(ImGui::CalcItemRectClosestPoint((ImVec2&)(pos), on_edge, outward));
		}

		float2 calcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width) noexcept
		{
			return ImVec2ToFloat2(ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width));
		}

		void calcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept
		{
			return ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
		}

		float4 colorConvertU32ToFloat4(std::uint32_t in) noexcept
		{
			return ImVec4ToFloat4(ImGui::ColorConvertU32ToFloat4(in));
		}

		std::uint32_t colorConvertFloat4ToU32(const float4& in) noexcept
		{
			return ImGui::ColorConvertFloat4ToU32((const ImVec4&)in);
		}

		void colorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept
		{
			return ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
		}

		void colorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept
		{
			return ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
		}

		bool isKeyDown(int key_index) noexcept
		{
			return ImGui::IsKeyDown(key_index);
		}

		bool isKeyPressed(int key_index, bool repeat) noexcept
		{
			return ImGui::IsKeyPressed(key_index, repeat);
		}

		bool isKeyReleased(int key_index) noexcept
		{
			return ImGui::IsKeyReleased(key_index);
		}

		bool isMouseDown(int button) noexcept
		{
			return ImGui::IsMouseDown(button);
		}

		bool isMouseClicked(int button, bool repeat) noexcept
		{
			return ImGui::IsMouseClicked(button, repeat);
		}

		bool isMouseDoubleClicked(int button) noexcept
		{
			return ImGui::IsMouseDoubleClicked(button);
		}

		bool isMouseReleased(int button) noexcept
		{
			return ImGui::IsMouseReleased(button);
		}

		bool isMouseHoveringWindow() noexcept
		{
			return ImGui::IsMouseHoveringWindow();
		}

		bool isMouseHoveringAnyWindow() noexcept
		{
			return ImGui::IsMouseHoveringAnyWindow();
		}

		bool isMouseHoveringRect(const float2& min, const float2& max, bool clip) noexcept
		{
			return ImGui::IsMouseHoveringRect((const ImVec2&)min, (const ImVec2&)max, clip);
		}

		bool isMouseDragging(int button, float lock_threshold) noexcept
		{
			return ImGui::IsMouseDragging();
		}

		float2 getMousePos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMousePos());
		}

		float2 getMousePosOnOpeningCurrentPopup() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMousePosOnOpeningCurrentPopup());
		}

		float2 getMouseDragDelta(int button, float lock_threshold) noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMouseDragDelta());
		}

		void resetMouseDragDelta(int button) noexcept
		{
			return ImGui::ResetMouseDragDelta();
		}

		void setMouseCursor(GuiMouseCursor type) noexcept
		{
			return ImGui::SetMouseCursor((ImGuiMouseCursor)type);
		}

		void captureKeyboardFromApp(bool capture) noexcept
		{
			return ImGui::CaptureKeyboardFromApp(capture);
		}

		void captureMouseFromApp(bool capture) noexcept
		{
			return ImGui::CaptureMouseFromApp(capture);
		}

		bool colorPicker3(const char* label, float col[3], GuiColorEditFlags flags )
		{
			return ImGui::ColorPicker3(label, col, flags);
		}

		bool colorPicker4(const char* label, float col[4], GuiColorEditFlags flags, const float* ref_col)
		{
			return ImGui::ColorPicker4(label, col, flags, ref_col);
		}

		bool sliderFloatWithRevert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = sliderFloat(label, v, _min, _max, display_format, power);
			if (!math::equal(*v, _default))
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name)) { *v = _default; change = true; };
				popID();
			}

			return change;
		}

		bool sliderFloat2WithRevert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = sliderFloat2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					change = false;
				};
				popID();
			}

			return change;
		}

		bool sliderFloat3WithRevert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = sliderFloat2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					change = false;
				};
				popID();
			}

			return change;
		}

		bool sliderFloat4WithRevert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = sliderFloat4(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2] || v[3] != _default[3])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					v[3] = _default[3];
					change = false;
				};
				popID();
			}

			return change;
		}

		bool sliderIntWithRevert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format) noexcept
		{
			bool change = sliderInt(label, v, _min, _max, display_format);
			if (v[0] != _default)
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name)) { *v = _default; change = true; };
				popID();
			}

			return change;
		}

		bool sliderInt2WithRevert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format) noexcept
		{
			bool change = sliderInt2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					change = true;
				};
				popID();
			}

			return change;
		}

		bool sliderInt3WithRevert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format) noexcept
		{
			bool change = sliderInt3(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					change = true;
				};
				popID();
			}

			return change;
		}

		bool sliderInt4WithRevert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format) noexcept
		{
			bool change = sliderInt4(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2] || v[3] != _default[3])
			{
				sameLine();
				pushID((int)std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					v[3] = _default[3];
					change = true;
				};
				popID();
			}

			return change;
		}

		void helpMarker(const char* text, const char* desc) noexcept
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(450.0f);
				ImGui::TextUnformatted(desc);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}

		float2 getDisplaySize() noexcept
		{
			return float2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		}

		void setStyle(const GuiStyle& newStyle) noexcept
		{
			static_assert(sizeof(ImGuiStyle::Colors) == sizeof(GuiStyle::Colors), "");

			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = newStyle.Alpha;
			style.WindowPadding.x = newStyle.WindowPadding.x;
			style.WindowPadding.y = newStyle.WindowPadding.y;
			style.WindowMinSize.x = newStyle.WindowMinSize.x;
			style.WindowMinSize.y = newStyle.WindowMinSize.y;
			style.WindowRounding = newStyle.WindowRounding;
			style.WindowTitleAlign.x = newStyle.WindowTitleAlign.x;
			style.WindowTitleAlign.y = newStyle.WindowTitleAlign.y;
			style.ChildBorderSize = newStyle.ChildBorderSize;
			style.ChildRounding = newStyle.ChildRounding;
			style.FramePadding.x = newStyle.FramePadding.x;
			style.FramePadding.y = newStyle.FramePadding.y;
			style.FrameRounding = newStyle.FrameRounding;
			style.ItemSpacing.x = newStyle.ItemSpacing.x;
			style.ItemSpacing.y = newStyle.ItemSpacing.y;
			style.ItemInnerSpacing.x = newStyle.ItemInnerSpacing.x;
			style.ItemInnerSpacing.y = newStyle.ItemInnerSpacing.y;
			style.TouchExtraPadding.x = newStyle.TouchExtraPadding.x;
			style.TouchExtraPadding.y = newStyle.TouchExtraPadding.y;
			style.IndentSpacing = newStyle.IndentSpacing;
			style.ColumnsMinSpacing = newStyle.ColumnsMinSpacing;
			style.ScrollbarSize = newStyle.ScrollbarSize;
			style.ScrollbarRounding = newStyle.ScrollbarRounding;
			style.GrabMinSize = newStyle.GrabMinSize;
			style.GrabRounding = newStyle.GrabRounding;
			style.ButtonTextAlign.x = newStyle.ButtonTextAlign.x;
			style.ButtonTextAlign.y = newStyle.ButtonTextAlign.y;
			style.DisplayWindowPadding.x = newStyle.DisplayWindowPadding.x;
			style.DisplayWindowPadding.y = newStyle.DisplayWindowPadding.y;
			style.DisplaySafeAreaPadding.x = newStyle.DisplaySafeAreaPadding.x;
			style.DisplaySafeAreaPadding.y = newStyle.DisplaySafeAreaPadding.y;
			style.AntiAliasedLines = newStyle.AntiAliasedLines;
			style.AntiAliasedFill = newStyle.AntiAliasedFill;
			style.CurveTessellationTol = newStyle.CurveTessellationTol;

			std::memcpy(&style.Colors, newStyle.Colors, sizeof(style.Colors));
		}

		const GuiStyle& getStyleDefault() noexcept
		{
			return _defalutStyle;
		}

		void rootDock(const float2& pos, const float2& size) noexcept
		{
			ImGui::RootDock((const ImVec2&)pos, (const ImVec2&)size);
		}

		bool beginDock(const char* label, bool* opened, GuiWindowFlags extra_flags, const float2& default_size) noexcept
		{
			if (!ImGui::BeginDock(label, opened, extra_flags, (ImVec2&)default_size))
			{
				ImGui::EndDock();
				return false;
			}

			return true;
		}

		void endDock() noexcept
		{
			ImGui::EndDock();
		}

		void setDockActive() noexcept
		{
			ImGui::SetDockActive();
		}

		bool dragFloatnEx(const char* labels[], float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloatNEx(labels, v, components, v_speed, v_min, v_max, display_format, power);
		}

		bool dragIntnEx(const char* labels[], int* v, int components, float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragIntNEx(labels, v, components, v_speed, v_min, v_max, display_format);
		}

		bool dragUIntnEx(const char* labels[], unsigned int* v, int components, float v_speed, unsigned int v_min, unsigned int v_max, const char* display_format) noexcept
		{
			return ImGui::DragUIntNEx(labels, v, components, v_speed, v_min, v_max, display_format);
		}

		void renderFrameEx(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness) noexcept
		{
			ImGui::RenderFrameEx((ImVec2&)p_min, (ImVec2&)p_max, border, rounding, thickness);
		}

		bool beginToolbar(const char* str_id, const float2& screen_pos, const float2& size) noexcept
		{
			return ImGui::BeginToolbar(str_id, (ImVec2&)screen_pos, (ImVec2&)size);
		}

		void endToolbar() noexcept
		{
			ImGui::EndToolbar();
		}

		bool toolbarButton(GuiTextureID texture, const char* tooltip, bool selected, bool enabled) noexcept
		{
			return ImGui::ToolbarButton(texture, tooltip, selected, enabled);
		}

		bool imageButtonEx(GuiTextureID texture, const float2& size, const char* tooltip, bool selected, bool enabled) noexcept
		{
			return ImGui::ImageButtonEx(texture, (ImVec2&)size, tooltip, selected, enabled);
		}

		bool imageButtonEx(GuiTextureID texture, const float2& size, bool enabled, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col) noexcept
		{
			if (enabled)
				return imageButton(texture, size, uv0, uv1, frame_padding, bg_col, tint_col);
			else
			{
				image(texture, size + float2(frame_padding * 2.0f, frame_padding * 2.0f), uv0, uv1, tint_col, bg_col);
				return false;
			}
		}

		bool imageButtonWithAspectAndLabel(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags) noexcept
		{
			return ImGui::ImageButtonWithAspectAndLabel(texture, (ImVec2&)texture_size, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, selected, edit_label, label, buf, buf_size, flags);
		}

		bool imageButtonAndLabel(const char* label, GuiTextureID texture, const float2& size, bool showLabel, bool selected, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col) noexcept
		{
			ImGui::BeginGroup();

			bool chlick = imageButtonEx(texture, size, true, uv0, uv1, frame_padding, bg_col, tint_col);

			if (selected)
			{
				ImVec2 rectMin = ImGui::GetItemRectMin();
				ImVec2 rectMax = ImGui::GetItemRectMax();
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.6f, 0.0f, 1.0f));
				ImGui::RenderFrameEx(rectMin, rectMax, true, 0.0f, 2.0f);
				ImGui::PopStyleColor();
			}

			if (label)
			{
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip(label);
			}

			if (label && showLabel)
			{
				ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size.x + frame_padding);
				ImGui::TextWrapped(label);
				ImGui::PopTextWrapPos();
			}

			ImGui::EndGroup();

			return chlick;
		}

		void imageWithAspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, const float4& tint_col, const float4& border_col) noexcept
		{
			ImGui::ImageWithAspect(texture, (ImVec2&)texture_size, (ImVec2&)size, (ImVec2&)size, (ImVec2&)uv0, (ImVec4&)tint_col, (ImVec4&)border_col);
		}

		void labelTextEx(const char* label, const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::LabelTextEx(label, fmt, args);
			va_end(args);
		}

		void labelTextExV(const char* label, const char* fmt, va_list args) noexcept
		{
			ImGui::LabelTextExV(label, fmt, args);
		}

		namespace guizmo
		{
			void SetDrawlist()
			{
				ImGuizmo::SetDrawlist();
			}

			void BeginFrame()
			{
				ImGuizmo::BeginFrame();
			}

			bool IsOver()
			{
				return ImGuizmo::IsOver();
			}

			bool IsUsing()
			{
				return ImGuizmo::IsUsing();
			}

			void Enable(bool enable)
			{
				ImGuizmo::Enable(enable);
			}

			void SetRect(float x, float y, float width, float height)
			{
				ImGuizmo::SetRect(x, y, width, height);
			}

			void Manipulate(const float *view, const float *projection, Operation operation, Mode mode, float *matrix, float *deltaMatrix, float *snap, float *localBounds, float *boundsSnap)
			{
				ImGuizmo::Manipulate(view, projection, (ImGuizmo::OPERATION)operation, (ImGuizmo::MODE)mode, matrix, deltaMatrix, snap, localBounds, boundsSnap);
			}
		}
	}
}