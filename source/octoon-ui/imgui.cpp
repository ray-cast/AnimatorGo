#include <octoon/ui/imgui.h>

#include <imgui.h>
#include <imgui_dock.h>
#include <imgui_internal.h>
#include <imgui_user.h>

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
			ChildWindowRounding = 0.0f;
			FramePadding = float2(0, 3);
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
			AntiAliasedShapes = true;
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
			Colors[ImGuiCol_CloseButton] = float4(0.2f, 0.5f, 1.0f, 0.5f);
			Colors[ImGuiCol_CloseButtonHovered] = float4(0.2f, 0.5f, 1.0f, 0.7f);
			Colors[ImGuiCol_CloseButtonActive] = float4(0.2f, 0.5f, 1.0f, 1.0f);
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
			Colors[ImGuiCol_ComboBg] = float4(0.20f, 0.20f, 0.20f, 0.99f);
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

		void new_frame() noexcept
		{
			ImGui::NewFrame();
		}

		void render() noexcept
		{
			ImGui::Render();
		}

		void shutdown() noexcept
		{
			ImGui::Shutdown();
		}

		void show_user_guide() noexcept
		{
			ImGui::ShowUserGuide();
		}

		void show_style_editor(GuiStyle* ref) noexcept
		{
			static_assert(sizeof(GuiStyle) == sizeof(ImGuiStyle));
			static_assert(offsetof(GuiStyle, Alpha) == offsetof(ImGuiStyle, Alpha));
			static_assert(offsetof(GuiStyle, WindowPadding) == offsetof(ImGuiStyle, WindowPadding));
			static_assert(offsetof(GuiStyle, WindowMinSize) == offsetof(ImGuiStyle, WindowMinSize));
			static_assert(offsetof(GuiStyle, WindowRounding) == offsetof(ImGuiStyle, WindowRounding));
			static_assert(offsetof(GuiStyle, WindowTitleAlign) == offsetof(ImGuiStyle, WindowTitleAlign));
			static_assert(offsetof(GuiStyle, ChildWindowRounding) == offsetof(ImGuiStyle, ChildWindowRounding));
			static_assert(offsetof(GuiStyle, FramePadding) == offsetof(ImGuiStyle, FramePadding));
			static_assert(offsetof(GuiStyle, FrameRounding) == offsetof(ImGuiStyle, FrameRounding));
			static_assert(offsetof(GuiStyle, ItemSpacing) == offsetof(ImGuiStyle, ItemSpacing));
			static_assert(offsetof(GuiStyle, ItemInnerSpacing) == offsetof(ImGuiStyle, ItemInnerSpacing));
			static_assert(offsetof(GuiStyle, TouchExtraPadding) == offsetof(ImGuiStyle, TouchExtraPadding));
			static_assert(offsetof(GuiStyle, IndentSpacing) == offsetof(ImGuiStyle, IndentSpacing));
			static_assert(offsetof(GuiStyle, ColumnsMinSpacing) == offsetof(ImGuiStyle, ColumnsMinSpacing));
			static_assert(offsetof(GuiStyle, ScrollbarSize) == offsetof(ImGuiStyle, ScrollbarSize));
			static_assert(offsetof(GuiStyle, ScrollbarRounding) == offsetof(ImGuiStyle, ScrollbarRounding));
			static_assert(offsetof(GuiStyle, GrabMinSize) == offsetof(ImGuiStyle, GrabMinSize));
			static_assert(offsetof(GuiStyle, GrabRounding) == offsetof(ImGuiStyle, GrabRounding));
			static_assert(offsetof(GuiStyle, ButtonTextAlign) == offsetof(ImGuiStyle, ButtonTextAlign));
			static_assert(offsetof(GuiStyle, DisplayWindowPadding) == offsetof(ImGuiStyle, DisplayWindowPadding));
			static_assert(offsetof(GuiStyle, DisplaySafeAreaPadding) == offsetof(ImGuiStyle, DisplaySafeAreaPadding));
			static_assert(offsetof(GuiStyle, AntiAliasedLines) == offsetof(ImGuiStyle, AntiAliasedLines));
			static_assert(offsetof(GuiStyle, AntiAliasedShapes) == offsetof(ImGuiStyle, AntiAliasedShapes));
			static_assert(offsetof(GuiStyle, CurveTessellationTol) == offsetof(ImGuiStyle, CurveTessellationTol));

			if (ref)
				ImGui::ShowStyleEditor((ImGuiStyle*)ref);
			else
				ImGui::ShowStyleEditor((ImGuiStyle*)&_defalutStyle);
		}

		void show_test_window(bool* isOpened) noexcept
		{
			if (!isOpened)
				ImGui::ShowTestWindow();
			else
				ImGui::ShowTestWindow(isOpened);
		}

		void show_metrics_window(bool* isOpened) noexcept
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

		bool begin_child(const char* str_id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChild(str_id, (ImVec2&)size, border, extraFlags))
			{
				ImGui::EndChild();
				return false;
			}

			return true;
		}

		bool begin_child(GuiID id, const float2& size, bool border, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChild((ImGuiID)id, (ImVec2&)size, border, extraFlags))
			{
				ImGui::EndChild();
				return false;
			}

			return true;
		}

		bool begin_child_frame(GuiID id, const float2& size, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginChildFrame(id, (const ImVec2&)size, extraFlags))
			{
				ImGui::EndChildFrame();
				return false;
			}

			return true;
		}

		void end_child() noexcept
		{
			return ImGui::EndChild();
		}

		void end_child_frame() noexcept
		{
			return ImGui::EndChildFrame();
		}

		float2 get_content_region_max() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetContentRegionMax());
		}

		float2 get_content_region_avail() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetContentRegionAvail());
		}

		float get_content_region_avail_width() noexcept
		{
			return ImGui::GetContentRegionAvailWidth();
		}

		float2 get_window_content_region_min() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowContentRegionMin());
		}

		float2 get_window_content_region_max() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowContentRegionMax());
		}

		float get_window_contentRegionWidth() noexcept
		{
			return ImGui::GetWindowContentRegionWidth();
		}

		float2 get_window_pos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowPos());
		}

		float2 get_window_size() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetWindowSize());
		}

		float get_window_width() noexcept
		{
			return ImGui::GetWindowWidth();
		}

		float get_window_height() noexcept
		{
			return ImGui::GetWindowHeight();
		}

		bool is_window_collapsed() noexcept
		{
			return ImGui::IsWindowCollapsed();
		}

		void set_window_fontScale(float scale) noexcept
		{
			ImGui::SetWindowFontScale(scale);
		}

		void set_next_window_pos(const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowPos((const ImVec2&)pos, cond);
		}

		void set_next_window_pos_center(GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowPosCenter(cond);
		}

		void set_next_window_size(const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowSize((const ImVec2&)size);
		}

		void set_next_window_size_constraints(const float2& min, const float2& max) noexcept
		{
			ImGui::SetNextWindowSizeConstraints((const ImVec2&)min, (const ImVec2&)max);
		}

		void set_next_window_content_size(const float2& size) noexcept
		{
			ImGui::SetNextWindowContentSize((const ImVec2&)size);
		}

		void set_next_window_content_width(float width) noexcept
		{
			ImGui::SetNextWindowContentWidth(width);
		}

		void set_next_window_collapsed(bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetNextWindowCollapsed(collapsed, cond);
		}

		void set_next_window_focus() noexcept
		{
			ImGui::SetNextWindowFocus();
		}

		void set_window_pos(const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowPos((const ImVec2&)pos, cond);
		}

		void set_window_size(const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowSize((const ImVec2&)size, cond);
		}

		void set_window_collapsed(bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowCollapsed(collapsed, cond);
		}

		void set_window_focus() noexcept
		{
			ImGui::SetWindowFocus();
		}

		void set_window_pos(const char* name, const float2& pos, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowPos(name, (const ImVec2&)pos, cond);
		}

		void set_window_size(const char* name, const float2& size, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowSize(name, (const ImVec2&)size, cond);
		}

		void set_window_collapsed(const char* name, bool collapsed, GuiSetCondFlags cond) noexcept
		{
			ImGui::SetWindowCollapsed(name, collapsed, cond);
		}

		void set_window_focus(const char* name) noexcept
		{
			ImGui::SetWindowFocus();
		}

		float get_scroll_x() noexcept
		{
			return ImGui::GetScrollX();
		}

		float get_scroll_y() noexcept
		{
			return ImGui::GetScrollY();
		}

		float get_scroll_max_x() noexcept
		{
			return ImGui::GetScrollMaxX();
		}

		float get_scroll_max_y() noexcept
		{
			return ImGui::GetScrollMaxY();
		}

		void set_scroll_x(float scroll_x) noexcept
		{
			ImGui::SetScrollX(scroll_x);
		}

		void set_scroll_y(float scroll_y) noexcept
		{
			ImGui::SetScrollY(scroll_y);
		}

		void set_scroll_here(float center_y_ratio) noexcept
		{
			ImGui::SetScrollHere(center_y_ratio);
		}

		void set_scroll_from_pos_y(float pos_y, float center_y_ratio) noexcept
		{
			ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
		}

		void set_keyboard_focus_here(int offset) noexcept
		{
			ImGui::SetKeyboardFocusHere(offset);
		}

		void push_style_color(GuiCol idx, const float4& col) noexcept
		{
			ImGui::PushStyleColor((ImGuiCol)idx, (ImVec4&)col);
		}

		void pop_style_color(int count) noexcept
		{
			ImGui::PopStyleColor(count);
		}

		void push_style_var(GuiStyleVar idx, float val) noexcept
		{
			ImGui::PushStyleVar((ImGuiStyleVar)idx, val);
		}

		void push_style_var(GuiStyleVar idx, const float2& val) noexcept
		{
			ImGui::PushStyleVar((ImGuiStyleVar)idx, (ImVec2&)val);
		}

		void pop_style_var(int count) noexcept
		{
			ImGui::PopStyleVar(count);
		}

		float get_font_size() noexcept
		{
			return ImGui::GetFontSize();
		}

		float2 get_font_tex_uv_white_pixel() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetFontTexUvWhitePixel());
		}

		std::uint32_t get_color_u32(GuiCol idx, float alpha_mul) noexcept
		{
			return ImGui::GetColorU32((ImGuiCol)idx, alpha_mul);
		}

		std::uint32_t get_color_u32(const float4& col) noexcept
		{
			return ImGui::GetColorU32((ImVec4&)col);
		}

		void push_item_width(float item_width) noexcept
		{
			ImGui::PushItemWidth(item_width);
		}

		void pop_item_width() noexcept
		{
			ImGui::PopItemWidth();
		}

		float calc_item_width() noexcept
		{
			return ImGui::CalcItemWidth();
		}

		void push_text_wrap_pos(float wrap_pos_x) noexcept
		{
			ImGui::PushTextWrapPos();
		}

		void pop_text_wrap_pos() noexcept
		{
			ImGui::PopTextWrapPos();
		}

		void push_allow_keyboard_focus(bool v) noexcept
		{
			ImGui::PushAllowKeyboardFocus(v);
		}

		void pop_allow_keyboard_focus() noexcept
		{
			ImGui::PopAllowKeyboardFocus();
		}

		void push_button_repeat(bool repeat) noexcept
		{
			ImGui::PushButtonRepeat(repeat);
		}

		void pop_button_repeat() noexcept
		{
			ImGui::PopButtonRepeat();
		}

		void separator() noexcept
		{
			ImGui::Separator();
		}

		void same_line(float pos_x, float spacing_w) noexcept
		{
			ImGui::SameLine(pos_x, spacing_w);
		}

		void new_line() noexcept
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

		void begin_group() noexcept
		{
			ImGui::BeginGroup();
		}

		void end_group() noexcept
		{
			ImGui::EndGroup();
		}

		float2 get_cursor_pos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorPos());
		}

		float get_cursor_pos_x() noexcept
		{
			return ImGui::GetCursorPosX();
		}

		float get_cursor_pos_y() noexcept
		{
			return ImGui::GetCursorPosY();
		}

		float2 get_cursor_start_pos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorStartPos());
		}

		float2 get_cursor_screen_pos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetCursorScreenPos());
		}

		void set_cursor_pos(const float2& local_pos) noexcept
		{
			ImGui::SetCursorPos((ImVec2&)local_pos);
		}

		void set_cursor_pos_x(float x) noexcept
		{
			ImGui::SetCursorPosX(x);
		}

		void set_cursor_pos_y(float y) noexcept
		{
			ImGui::SetCursorPosY(y);
		}

		void set_cursor_screen_pos(const float2& pos) noexcept
		{
			return ImGui::SetCursorScreenPos((ImVec2&)pos);
		}

		void align_first_text_height_to_widgets() noexcept
		{
			ImGui::AlignFirstTextHeightToWidgets();
		}

		float get_text_line_height() noexcept
		{
			return ImGui::GetTextLineHeight();
		}

		float get_text_line_height_with_spacing() noexcept
		{
			return ImGui::GetTextLineHeightWithSpacing();
		}

		float get_items_line_height_with_spacing() noexcept
		{
			return ImGui::GetItemsLineHeightWithSpacing();
		}

		void columns(int count, const char* id, bool border) noexcept
		{
			ImGui::Columns(count, id, border);
		}

		void push_id(const char* str_id_begin, const char* str_id_end) noexcept
		{
			ImGui::PushID(str_id_begin, str_id_end);
		}

		void push_id(const void* ptr_id) noexcept
		{
			ImGui::PushID(ptr_id);
		}

		void push_id(int int_id) noexcept
		{
			ImGui::PushID(int_id);
		}

		void pop_id() noexcept
		{
			ImGui::PopID();
		}

		GuiID get_id(const char* str_id) noexcept
		{
			return ImGui::GetID(str_id);
		}

		GuiID get_id(const char* str_id_begin, const char* str_id_end) noexcept
		{
			return ImGui::GetID(str_id_begin, str_id_end);
		}

		GuiID get_id(const void* ptr_id) noexcept
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

		void text_v(const char* fmt, va_list args) noexcept
		{
			ImGui::TextV(fmt, args);
		}

		void text_colored(const float4& col, const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextColoredV((ImVec4&)col, fmt, args);
			va_end(args);
		}

		void text_colored_v(const float4& col, const char* fmt, va_list args) noexcept
		{
			ImGui::TextColoredV((ImVec4&)col, fmt, args);
		}

		void text_disabled(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextDisabledV(fmt, args);
			va_end(args);
		}

		void text_disabled_v(const char* fmt, va_list args) noexcept
		{
			ImGui::TextDisabledV(fmt, args);
		}

		void text_wrapped(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::TextWrappedV(fmt, args);
			va_end(args);
		}

		void text_wrapped_v(const char* fmt, va_list args) noexcept
		{
			ImGui::TextWrappedV(fmt, args);
		}

		void text_unformatted(const char* text, const char* text_end) noexcept
		{
			ImGui::TextUnformatted(text, text_end);
		}

		void label_text(const char* label, const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::LabelTextV(label, fmt, args);
			va_end(args);
		}

		void label_text_v(const char* label, const char* fmt, va_list args) noexcept
		{
			ImGui::LabelTextV(label, fmt, args);
		}

		void bullet() noexcept
		{
			ImGui::Bullet();
		}

		void bullet_text(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::BulletTextV(fmt, args);
			va_end(args);
		}

		void bullet_text_v(const char* fmt, va_list args) noexcept
		{
			ImGui::BulletTextV(fmt, args);
		}

		bool button(const char* label, const float2& size) noexcept
		{
			return ImGui::Button(label, (ImVec2&)size);
		}

		bool small_button(const char* label) noexcept
		{
			return ImGui::SmallButton(label);
		}

		bool invisible_button(const char* str_id, const float2& size) noexcept
		{
			return ImGui::InvisibleButton(str_id, (ImVec2&)size);
		}

		void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, const float4& tint_col, const float4& border_col) noexcept
		{
			ImGui::Image(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, (ImVec4&)tint_col, (ImVec4&)border_col);
		}

		bool image_button(GuiTextureID user_texture_id, const float2& size, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col) noexcept
		{
			return ImGui::ImageButton(user_texture_id, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, frame_padding, (ImVec4&)bg_col, (ImVec4&)tint_col);
		}

		bool checkbox(const char* label, bool* v) noexcept
		{
			return ImGui::Checkbox(label, v);
		}

		bool checkbox_flags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept
		{
			return ImGui::CheckboxFlags(label, flags, flags_value);
		}

		bool radio_button(const char* label, bool active) noexcept
		{
			return ImGui::RadioButton(label, active);
		}

		bool radio_button(const char* label, int* v, int v_button) noexcept
		{
			return ImGui::RadioButton(label, v, v_button);
		}

		bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
		{
			return ImGui::Combo(label, current_item, items, items_count, height_in_items);
		}

		bool combo_with_revert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items) noexcept
		{
			bool change = ImGui::Combo(label, current_item, items, items_count, height_in_items);
			if (*current_item != _default)
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(revert)) { *current_item = _default; change = false; };
				pop_id();
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

		bool color_button(const float4& col, bool small_height, bool outline_border) noexcept
		{
			return ImGui::ColorButton((ImVec4&)col, small_height, outline_border);
		}

		bool color_edit3(const char* label, float col[3]) noexcept
		{
			return ImGui::ColorEdit3(label, col);
		}

		bool color_edit4(const char* label, float col[4], bool show_alpha) noexcept
		{
			return ImGui::ColorEdit4(label, col, show_alpha);
		}

		void color_edit_mode(GuiColorEditMode mode) noexcept
		{
			ImGui::ColorEditMode((ImGuiColorEditMode)mode);
		}

		void plot_lines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
		{
			ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
		}

		void plot_lines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
		{
			ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size);
		}

		void plot_histogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size, int stride) noexcept
		{
			ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, (ImVec2&)graph_size, stride);
		}

		void plot_histogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, const float2& graph_size) noexcept
		{
			ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max);
		}

		void progress_bar(float fraction, const float2& size_arg, const char* overlay) noexcept
		{
			ImGui::ProgressBar(fraction, (ImVec2&)size_arg, overlay);
		}

		bool drag_float(const char* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool drag_float2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool drag_float3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool drag_float4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, display_format, power);
		}

		bool drag_float_range2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power) noexcept
		{
			return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
		}

		bool drag_int(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt(label, v, v_speed, v_min, v_max, display_format);
		}

		bool drag_int2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt2(label, v, v_speed, v_min, v_max, display_format);
		}

		bool drag_int3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt3(label, v, v_speed, v_min, v_max, display_format);
		}

		bool drag_int4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::DragInt4(label, v, v_speed, v_min, v_max, display_format);
		}

		bool drag_int_range2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max) noexcept
		{
			return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format);
		}

		bool input_text(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags) noexcept
		{
			return ImGui::InputText(label, buf, buf_size, flags);
		}

		bool input_text_multiline(const char* label, char* buf, size_t buf_size, const float2& size, GuiInputTextFlags flags) noexcept
		{
			return ImGui::InputTextMultiline(label, buf, buf_size, (const ImVec2&)size, flags);
		}

		bool input_float(const char* label, float* v, float step, float step_fast, int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extraFlags);
		}

		bool input_float2(const char* label, float v[2], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat2(label, v, decimal_precision, extraFlags);
		}

		bool input_float3(const char* label, float v[3], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat3(label, v, decimal_precision, extraFlags);
		}

		bool input_float4(const char* label, float v[4], int decimal_precision, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputFloat4(label, v, decimal_precision, extraFlags);
		}

		bool input_int(const char* label, int* v, int step, int step_fast, GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt(label, v, step, step_fast, extraFlags);
		}

		bool input_int2(const char* label, int v[2], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt2(label, v, extraFlags);
		}

		bool input_int3(const char* label, int v[3], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt3(label, v, extraFlags);
		}

		bool input_int4(const char* label, int v[4], GuiInputTextFlags extraFlags) noexcept
		{
			return ImGui::InputInt4(label, v, extraFlags);
		}

		bool slider_float(const char* label, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat(label, v, v_min, v_max, display_format, power);
		}

		bool slider_float2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat2(label, v, v_min, v_max, display_format, power);
		}

		bool slider_float3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat3(label, v, v_min, v_max, display_format, power);
		}

		bool slider_float4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::SliderFloat4(label, v, v_min, v_max, display_format, power);
		}

		bool slider_floatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format, float power) noexcept
		{
			return ImGui::VSliderFloat(label, (ImVec2&)size, v, v_min, v_max, display_format, power);
		}

		bool slider_angle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max) noexcept
		{
			return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
		}

		bool slider_int(const char* label, int* v, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt(label, v, v_min, v_max, display_format);
		}

		bool slider_int2(const char* label, int v[2], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt2(label, v, v_min, v_max, display_format);
		}

		bool slider_int3(const char* label, int v[3], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt3(label, v, v_min, v_max, display_format);
		}

		bool slider_int4(const char* label, int v[4], int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::SliderInt4(label, v, v_min, v_max, display_format);
		}

		bool slider_intv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format) noexcept
		{
			return ImGui::VSliderInt(label, (ImVec2&)size, v, v_min, v_max, display_format);
		}

		bool tree_node(const char* label) noexcept
		{
			return ImGui::TreeNode(label);
		}

		bool tree_node(const char* str_id, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeExV(str_id, 0, fmt, args);
			va_end(args);
			return result;
		}

		bool tree_node(const void* ptr_id, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
			va_end(args);
			return result;
		}

		bool tree_node_v(const char* str_id, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeExV(str_id, 0, fmt, args);
		}

		bool tree_node_v(const void* ptr_id, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeExV(ptr_id, 0, fmt, args);
		}

		bool tree_node_ex(const char* label, GuiTreeNodeFlags flags) noexcept
		{
			return ImGui::TreeNodeEx(label, flags);
		}

		bool tree_node_ex(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeEx(str_id, flags, fmt, args);
			va_end(args);
			return result;
		}

		bool tree_node_ex(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept
		{
			bool result = false;
			va_list args;
			va_start(args, fmt);
			result = ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
			va_end(args);
			return result;
		}

		bool tree_node_ex_v(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeEx(str_id, flags, fmt, args);
		}

		bool tree_node_ex_v(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept
		{
			return ImGui::TreeNodeEx(ptr_id, flags, fmt, args);
		}

		void tree_push(const char* str_id) noexcept
		{
			return ImGui::TreePush(str_id);
		}

		void tree_push(const void* ptr_id) noexcept
		{
			return ImGui::TreePush(ptr_id);
		}

		void tree_pop() noexcept
		{
			return ImGui::TreePop();
		}

		void tree_advance_to_label_pos() noexcept
		{
			return ImGui::TreeAdvanceToLabelPos();
		}

		float get_tree_node_to_label_spacing() noexcept
		{
			return ImGui::GetTreeNodeToLabelSpacing();
		}

		void set_next_tree_node_open(bool is_open, GuiSetCondFlags cond) noexcept
		{
			return ImGui::SetNextTreeNodeOpen(is_open, cond);
		}

		bool collapsing_header(const char* label, GuiTreeNodeFlags flags) noexcept
		{
			return ImGui::CollapsingHeader(label, flags);
		}

		bool collapsing_header(const char* label, bool* isOpened, GuiTreeNodeFlags flags) noexcept
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

		bool list_box(const char* label, int* current_item, const char** items, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
		}

		bool list_box(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items);
		}

		bool list_box_header(const char* label, const float2& size) noexcept
		{
			return ImGui::ListBoxHeader(label, (const ImVec2&)size);
		}

		bool list_box_header(const char* label, int items_count, int height_in_items) noexcept
		{
			return ImGui::ListBoxHeader(label, items_count, height_in_items);
		}

		void list_box_footer() noexcept
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

		void value_color(const char* prefix, const float4& v) noexcept
		{
			ImGui::ValueColor(prefix, (const ImVec4&)v);
		}

		void value_color(const char* prefix, std::uint32_t v) noexcept
		{
			ImGui::ValueColor(prefix, v);
		}

		void set_tooltip(const char* fmt, ...) noexcept
		{
			va_list args;
			va_start(args, fmt);
			ImGui::SetTooltipV(fmt, args);
			va_end(args);
		}

		void set_tooltipV(const char* fmt, va_list args) noexcept
		{
			ImGui::SetTooltipV(fmt, args);
		}

		void begin_tooltip() noexcept
		{
			ImGui::BeginTooltip();
		}

		void end_tooltip() noexcept
		{
			ImGui::EndTooltip();
		}

		bool begin_main_menu_bar() noexcept
		{
			if (!ImGui::BeginMainMenuBar())
			{
				ImGui::EndMainMenuBar();
				return false;
			}

			return true;
		}

		void end_main_menu_bar() noexcept
		{
			ImGui::EndMainMenuBar();
		}

		bool begin_menu_bar() noexcept
		{
			return ImGui::BeginMenuBar();
		}

		void end_menu_bar() noexcept
		{
			return ImGui::EndMenuBar();
		}

		bool begin_menu(const char* label, bool enabled) noexcept
		{
			return ImGui::BeginMenu(label, enabled);
		}

		void end_menu() noexcept
		{
			return ImGui::EndMenu();
		}

		bool menu_item(const char* label, const char* shortcut, bool selected, bool enabled) noexcept
		{
			return ImGui::MenuItem(label, shortcut, selected, enabled);
		}

		bool menu_item(const char* label, const char* shortcut, bool* p_selected, bool enabled) noexcept
		{
			return ImGui::MenuItem(label, shortcut, p_selected, enabled);
		}

		bool begin_popup(const char* str_id) noexcept
		{
			return ImGui::BeginPopup(str_id);
		}

		bool begin_popup_modal(const char* name, bool* isOpened, GuiWindowFlags extraFlags) noexcept
		{
			if (!ImGui::BeginPopupModal(name, isOpened, extraFlags))
			{
				return *isOpened = false;
			}

			return true;
		}

		bool begin_popup_context_item(const char* str_id, int mouse_button) noexcept
		{
			return ImGui::BeginPopupContextItem(str_id, mouse_button);
		}

		bool begin_popup_context_window(bool also_over_items, const char* str_id, int mouse_button) noexcept
		{
			return ImGui::BeginPopupContextWindow(also_over_items, str_id, mouse_button);
		}

		bool begin_popup_context_void(const char* str_id, int mouse_button) noexcept
		{
			return ImGui::BeginPopupContextVoid(str_id, mouse_button);
		}

		void open_popup(const char* str_id) noexcept
		{
			return ImGui::OpenPopup(str_id);
		}

		void close_current_popup() noexcept
		{
			return ImGui::CloseCurrentPopup();
		}

		void end_popup() noexcept
		{
			return ImGui::EndPopup();
		}

		void push_clip_rect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept
		{
			return ImGui::PushClipRect((const ImVec2&)clip_rect_min, (const ImVec2&)clip_rect_max, intersect_with_current_clip_rect);
		}

		void pop_clip_rect() noexcept
		{
			return ImGui::PopClipRect();
		}

		bool is_item_hovered() noexcept
		{
			return ImGui::IsItemHovered();
		}

		bool is_item_hovered_rect() noexcept
		{
			return ImGui::IsItemHoveredRect();
		}

		bool is_item_active() noexcept
		{
			return ImGui::IsItemActive();
		}

		bool is_item_clicked(int mouse_button) noexcept
		{
			return ImGui::IsItemClicked();
		}

		bool is_item_visible() noexcept
		{
			return ImGui::IsItemVisible();
		}

		bool is_any_item_hovered() noexcept
		{
			return ImGui::IsAnyItemHovered();
		}

		bool is_any_item_active() noexcept
		{
			return ImGui::IsAnyItemActive();
		}

		float2 get_item_rect_min() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectMin());
		}

		float2 get_item_rect_max() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectMax());
		}

		float2 get_item_rect_size() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetItemRectSize());
		}

		void set_item_allow_overlap() noexcept
		{
			return ImGui::SetItemAllowOverlap();
		}

		bool is_window_hovered() noexcept
		{
			return ImGui::IsWindowHovered();
		}

		bool is_window_focused() noexcept
		{
			return ImGui::IsWindowFocused();
		}

		bool is_root_window_focused() noexcept
		{
			return ImGui::IsRootWindowFocused();
		}

		bool is_root_window_or_any_child_focused() noexcept
		{
			return ImGui::IsRootWindowOrAnyChildFocused();
		}

		bool is_root_window_or_any_child_hovered() noexcept
		{
			return ImGui::IsRootWindowOrAnyChildHovered();
		}

		bool is_rect_visible(const float2& size) noexcept
		{
			return ImGui::IsRectVisible((const ImVec2&)size);
		}

		bool is_pos_hovering_any_window(const float2& pos) noexcept
		{
			return ImGui::IsPosHoveringAnyWindow((const ImVec2&)pos);
		}

		float get_time() noexcept
		{
			return ImGui::GetTime();
		}

		int get_frame_count() noexcept
		{
			return ImGui::GetFrameCount();
		}

		const char* get_style_col_name(GuiCol idx) noexcept
		{
			return ImGui::GetStyleColName((ImGuiCol)idx);
		}

		float2 calc_item_rect_closest_point(const float2& pos, bool on_edge, float outward) noexcept
		{
			return ImVec2ToFloat2(ImGui::CalcItemRectClosestPoint((ImVec2&)(pos), on_edge, outward));
		}

		float2 calc_text_size(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width) noexcept
		{
			return ImVec2ToFloat2(ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width));
		}

		void calc_list_clipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept
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

		void color_convertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept
		{
			return ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
		}

		void color_convertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept
		{
			return ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
		}

		bool is_key_down(int key_index) noexcept
		{
			return ImGui::IsKeyDown(key_index);
		}

		bool is_key_pressed(int key_index, bool repeat) noexcept
		{
			return ImGui::IsKeyPressed(key_index, repeat);
		}

		bool is_key_released(int key_index) noexcept
		{
			return ImGui::IsKeyReleased(key_index);
		}

		bool is_mouse_down(int button) noexcept
		{
			return ImGui::IsMouseDown(button);
		}

		bool is_mouse_clicked(int button, bool repeat) noexcept
		{
			return ImGui::IsMouseClicked(button, repeat);
		}

		bool is_mouse_double_clicked(int button) noexcept
		{
			return ImGui::IsMouseDoubleClicked(button);
		}

		bool is_mouse_released(int button) noexcept
		{
			return ImGui::IsMouseReleased(button);
		}

		bool is_mouse_hovering_window() noexcept
		{
			return ImGui::IsMouseHoveringWindow();
		}

		bool is_mouse_hovering_any_window() noexcept
		{
			return ImGui::IsMouseHoveringAnyWindow();
		}

		bool is_mouse_hovering_rect(const float2& min, const float2& max, bool clip) noexcept
		{
			return ImGui::IsMouseHoveringRect((const ImVec2&)min, (const ImVec2&)max, clip);
		}

		bool is_mouse_dragging(int button, float lock_threshold) noexcept
		{
			return ImGui::IsMouseDragging();
		}

		float2 get_mouse_pos() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMousePos());
		}

		float2 get_mouse_pos_on_opening_current_popup() noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMousePosOnOpeningCurrentPopup());
		}

		float2 get_mouse_drag_delta(int button, float lock_threshold) noexcept
		{
			return ImVec2ToFloat2(ImGui::GetMouseDragDelta());
		}

		void reset_mouse_drag_delta(int button) noexcept
		{
			return ImGui::ResetMouseDragDelta();
		}

		void set_cursor_icon(GuiMouseCursor type) noexcept
		{
			return ImGui::SetMouseCursor((ImGuiMouseCursor)type);
		}

		void capture_keyboard_from_app(bool capture) noexcept
		{
			return ImGui::CaptureKeyboardFromApp(capture);
		}

		void capture_mouse_from_app(bool capture) noexcept
		{
			return ImGui::CaptureMouseFromApp(capture);
		}

		bool color_picker3(const char* label, float col[3], const float2& size, float hueSize, float crossHairSize) noexcept
		{
			// thanks to : https://github.com/ocornut/imgui/issues/346
			bool value_changed = false;

			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImVec2 picker_pos = ImGui::GetCursorScreenPos();

			ImColor color(col[0], col[1], col[2]);
			ImColor colors[] = { ImColor(255, 0, 0), ImColor(255, 255, 0), ImColor(0, 255, 0), ImColor(0, 255, 255), ImColor(0, 0, 255), ImColor(255, 0, 255), ImColor(255, 0, 0) };

			for (int i = 0; i < 6; ++i)
			{
				draw_list->AddRectFilledMultiColor(
					ImVec2(picker_pos.x + size.x + 10, picker_pos.y + i * (size.y / 6)),
					ImVec2(picker_pos.x + size.x + 10 + hueSize,
						picker_pos.y + (i + 1) * (size.y / 6)),
					colors[i],
					colors[i],
					colors[i + 1],
					colors[i + 1]);
			}

			float hue, saturation, value;
			ImGui::ColorConvertRGBtoHSV(color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

			draw_list->AddLine(
				ImVec2(picker_pos.x + size.x + 8, picker_pos.y + hue * size.y),
				ImVec2(picker_pos.x + size.x + 12 + hueSize, picker_pos.y + hue * size.y),
				ImColor(255, 255, 255));

			{
				const int step = 5;
				ImVec2 pos = ImVec2(0, 0);

				ImVec4 c00(1, 1, 1, 1);
				ImVec4 c10(1, 1, 1, 1);
				ImVec4 c01(1, 1, 1, 1);
				ImVec4 c11(1, 1, 1, 1);

				for (int y = 0; y < step; y++)
				{
					for (int x = 0; x < step; x++)
					{
						float s0 = (float)x / (float)step;
						float s1 = (float)(x + 1) / (float)step;
						float v0 = 1.0 - (float)(y) / (float)step;
						float v1 = 1.0 - (float)(y + 1) / (float)step;

						ImGui::ColorConvertHSVtoRGB(hue, s0, v0, c00.x, c00.y, c00.z);
						ImGui::ColorConvertHSVtoRGB(hue, s1, v0, c10.x, c10.y, c10.z);
						ImGui::ColorConvertHSVtoRGB(hue, s0, v1, c01.x, c01.y, c01.z);
						ImGui::ColorConvertHSVtoRGB(hue, s1, v1, c11.x, c11.y, c11.z);

						draw_list->AddRectFilledMultiColor(
							ImVec2(picker_pos.x + pos.x, picker_pos.y + pos.y),
							ImVec2(picker_pos.x + pos.x + size.x / step, picker_pos.y + pos.y + size.y / step),
							ImGui::ColorConvertFloat4ToU32(c00),
							ImGui::ColorConvertFloat4ToU32(c10),
							ImGui::ColorConvertFloat4ToU32(c11),
							ImGui::ColorConvertFloat4ToU32(c01));

						pos.x += size.x / step;
					}
					pos.x = 0;
					pos.y += size.y / step;
				}
			}

			float x = saturation * size.x;
			float y = (1 - value) * size.y;
			ImVec2 p(picker_pos.x + x, picker_pos.y + y);
			draw_list->AddLine(ImVec2(p.x - crossHairSize, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
			draw_list->AddLine(ImVec2(p.x + crossHairSize, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
			draw_list->AddLine(ImVec2(p.x, p.y + crossHairSize), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
			draw_list->AddLine(ImVec2(p.x, p.y - crossHairSize), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

			invisible_button("saturation_value_selector", size);

			if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

				/**/ if (mouse_pos_in_canvas.x < 0) mouse_pos_in_canvas.x = 0;
				else if (mouse_pos_in_canvas.x >= size.x - 1) mouse_pos_in_canvas.x = size.x - 1;

				/**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
				else if (mouse_pos_in_canvas.y >= size.y - 1) mouse_pos_in_canvas.y = size.y - 1;

				value = 1 - (mouse_pos_in_canvas.y / (size.y - 1));
				saturation = mouse_pos_in_canvas.x / (size.x - 1);
				value_changed = true;
			}

			ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + size.x + 10, picker_pos.y));
			ImGui::InvisibleButton("hue_selector", ImVec2(hueSize, size.y));

			if ((ImGui::IsItemHovered() || ImGui::IsItemActive()) && ImGui::GetIO().MouseDown[0])
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

				/* Previous horizontal bar will represent hue=1 (bottom) as hue=0 (top). Since both colors are red, we clamp at (-2, above edge) to avoid visual continuities */
				/**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
				else if (mouse_pos_in_canvas.y >= size.y - 2) mouse_pos_in_canvas.y = size.y - 2;

				hue = mouse_pos_in_canvas.y / (size.y - 1);
				value_changed = true;
			}

			color = ImColor::HSV(hue > 0 ? hue : 1e-6, saturation > 0 ? saturation : 1e-6, value > 0 ? value : 1e-6);

			col[0] = ImClamp(color.Value.x, 0.0f, 1.0f);
			col[1] = ImClamp(color.Value.y, 0.0f, 1.0f);
			col[2] = ImClamp(color.Value.z, 0.0f, 1.0f);

			return value_changed | ImGui::ColorEdit3(label, col);
		}

		bool color_picker3_with_revert(const char* label, const char* name, float col[3], const float _default[3], const float2& size, float hueSize, float crossHairSize) noexcept
		{
			bool change = color_picker3(label, col, size, hueSize, crossHairSize);
			if (!math::equal(col[0], _default[0]) || !math::equal(col[1], _default[1]) || !math::equal(col[2], _default[2]))
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					col[0] = _default[0];
					col[1] = _default[1];
					col[2] = _default[2];
					change = false;
				};

				pop_id();
			}

			return change;
		}

		bool slider_float_with_revert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = slider_float(label, v, _min, _max, display_format, power);
			if (!math::equal(*v, _default))
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name)) { *v = _default; change = true; };
				pop_id();
			}

			return change;
		}

		bool slider_float2_with_revert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = slider_float2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					change = false;
				};
				pop_id();
			}

			return change;
		}

		bool slider_float3_with_revert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = slider_float2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					change = false;
				};
				pop_id();
			}

			return change;
		}

		bool slider_float4_with_revert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format, float power) noexcept
		{
			bool change = slider_float4(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2] || v[3] != _default[3])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					v[3] = _default[3];
					change = false;
				};
				pop_id();
			}

			return change;
		}

		bool slider_int_with_revert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format) noexcept
		{
			bool change = slider_int(label, v, _min, _max, display_format);
			if (v[0] != _default)
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name)) { *v = _default; change = true; };
				pop_id();
			}

			return change;
		}

		bool slider_int2_with_revert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format) noexcept
		{
			bool change = slider_int2(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					change = true;
				};
				pop_id();
			}

			return change;
		}

		bool slider_int3_with_revert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format) noexcept
		{
			bool change = slider_int3(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					change = true;
				};
				pop_id();
			}

			return change;
		}

		bool slider_int4_with_revert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format) noexcept
		{
			bool change = slider_int4(label, v, _min, _max, display_format);
			if (v[0] != _default[0] || v[1] != _default[1] || v[2] != _default[2] || v[3] != _default[3])
			{
				same_line();
				push_id(std::hash<const char*>{}(label));
				if (button(name))
				{
					v[0] = _default[0];
					v[1] = _default[1];
					v[2] = _default[2];
					v[3] = _default[3];
					change = true;
				};
				pop_id();
			}

			return change;
		}

		void help_marker(const char* text, const char* desc) noexcept
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

		float2 get_display_size() noexcept
		{
			return float2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
		}

		void set_style(const GuiStyle& newStyle) noexcept
		{
			static_assert(sizeof(ImGuiStyle::Colors) == sizeof(GuiStyle::Colors));

			ImGuiStyle& style = ImGui::GetStyle();
			style.Alpha = newStyle.Alpha;
			style.WindowPadding.x = newStyle.WindowPadding.x;
			style.WindowPadding.y = newStyle.WindowPadding.y;
			style.WindowMinSize.x = newStyle.WindowMinSize.x;
			style.WindowMinSize.y = newStyle.WindowMinSize.y;
			style.WindowRounding = newStyle.WindowRounding;
			style.WindowTitleAlign.x = newStyle.WindowTitleAlign.x;
			style.WindowTitleAlign.y = newStyle.WindowTitleAlign.y;
			style.ChildWindowRounding = newStyle.ChildWindowRounding;
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
			style.AntiAliasedShapes = newStyle.AntiAliasedShapes;
			style.CurveTessellationTol = newStyle.CurveTessellationTol;

			std::memcpy(&style.Colors, newStyle.Colors, sizeof(style.Colors));
		}

		const GuiStyle& get_style_default() noexcept
		{
			return _defalutStyle;
		}

		void root_dock(const float2& pos, const float2& size)
		{
			ImGui::RootDock((const ImVec2&)pos, (const ImVec2&)size);
		}

		bool begin_dock(const char* label, bool* opened, GuiWindowFlags extra_flags, const float2& default_size)
		{
			if (!ImGui::BeginDock(label, opened, extra_flags, (ImVec2&)default_size))
			{
				ImGui::EndDock();
				return false;
			}

			return true;
		}

		void end_dock()
		{
			ImGui::EndDock();
		}

		void set_dock_active()
		{
			ImGui::SetDockActive();
		}

		bool drag_floatn_ex(const char* labels[], float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power)
		{
			return ImGui::DragFloatNEx(labels, v, components, v_speed, v_min, v_max, display_format, power);
		}

		bool drag_intn_ex(const char* labels[], int* v, int components, float v_speed, int v_min, int v_max, const char* display_format)
		{
			return ImGui::DragIntNEx(labels, v, components, v_speed, v_min, v_max, display_format);
		}

		bool drag_uIntn_ex(const char* labels[], unsigned int* v, int components, float v_speed, unsigned int v_min, unsigned int v_max, const char* display_format)
		{
			return ImGui::DragUIntNEx(labels, v, components, v_speed, v_min, v_max, display_format);
		}

		void render_frame_ex(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness)
		{
			ImGui::RenderFrameEx((ImVec2&)p_min, (ImVec2&)p_max, border, rounding, thickness);
		}

		bool begin_toolbar(const char* str_id, const float2& screen_pos, const float2& size)
		{
			return ImGui::BeginToolbar(str_id, (ImVec2&)screen_pos, (ImVec2&)size);
		}

		void end_toolbar()
		{
			ImGui::EndToolbar();
		}

		bool toolbar_button(GuiTextureID texture, const char* tooltip, bool selected, bool enabled)
		{
			return ImGui::ToolbarButton(texture, tooltip, selected, enabled);
		}

		bool image_button_ex(GuiTextureID texture, const float2& size, const char* tooltip, bool selected, bool enabled)
		{
			return ImGui::ImageButtonEx(texture, (ImVec2&)size, tooltip, selected, enabled);
		}

		bool image_button_ex(GuiTextureID texture, const float2& size, bool enabled, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col)
		{
			if (enabled)
				return image_button(texture, size, uv0, uv1, frame_padding, bg_col, tint_col);
			else
			{
				image(texture, size + float2(frame_padding * 2.0f, frame_padding * 2.0f), uv0, uv1, tint_col, bg_col);
				return false;
			}
		}

		bool image_button_with_aspect_and_label(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags)
		{
			return ImGui::ImageButtonWithAspectAndLabel(texture, (ImVec2&)texture_size, (ImVec2&)size, (ImVec2&)uv0, (ImVec2&)uv1, selected, edit_label, label, buf, buf_size, flags);
		}

		bool image_button_and_label(const char* label, GuiTextureID texture, const float2& size, bool showLabel, bool selected, const float2& uv0, const float2& uv1, int frame_padding, const float4& bg_col, const float4& tint_col)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			ImGui::BeginGroup();

			bool chlick = image_button_ex(texture, size, true, uv0, uv1, frame_padding, bg_col, tint_col);

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

		void image_with_aspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, const float4& tint_col, const float4& border_col)
		{
			ImGui::ImageWithAspect(texture, (ImVec2&)texture_size, (ImVec2&)size, (ImVec2&)size, (ImVec2&)uv0, (ImVec4&)tint_col, (ImVec4&)border_col);
		}

		void label_text_ex(const char* label, const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			ImGui::LabelTextEx(label, fmt, args);
			va_end(args);
		}

		void label_text_ex_v(const char* label, const char* fmt, va_list args)
		{
			ImGui::LabelTextExV(label, fmt, args);
		}
	}
}