#ifndef OCTOON_IMGUI_H_
#define OCTOON_IMGUI_H_

#include <octoon/math/math.h>

namespace octoon
{
	namespace imgui
	{
		using float2 = math::float2;
		using float3 = math::float3;
		using float4 = math::float4;

		enum GuiCol
		{
			GuiColText,
			GuiColTextDisabled,
			GuiColWindowBg,
			GuiColChildWindowBg,
			GuiColPopupBg,
			GuiColBorder,
			GuiColBorderShadow,
			GuiColFrameBg,
			GuiColFrameBgHovered,
			GuiColFrameBgActive,
			GuiColTitleBg,
			GuiColTitleBgCollapsed,
			GuiColTitleBgActive,
			GuiColMenuBarBg,
			GuiColScrollbarBg,
			GuiColScrollbarGrab,
			GuiColScrollbarGrabHovered,
			GuiColScrollbarGrabActive,
			GuiColComboBg,
			GuiColCheckMark,
			GuiColSliderGrab,
			GuiColSliderGrabActive,
			GuiColButton,
			GuiColButtonHovered,
			GuiColButtonActive,
			GuiColHeader,
			GuiColHeaderHovered,
			GuiColHeaderActive,
			GuiColColumn,
			GuiColColumnHovered,
			GuiColColumnActive,
			GuiColResizeGrip,
			GuiColResizeGripHovered,
			GuiColResizeGripActive,
			GuiColCloseButton,
			GuiColCloseButtonHovered,
			GuiColCloseButtonActive,
			GuiColPlotLines,
			GuiColPlotLinesHovered,
			GuiColPlotHistogram,
			GuiColPlotHistogramHovered,
			GuiColTextSelectedBg,
			GuiColModalWindowDarkening,  // darken entire screen when a modal window is active
			GuiColCOUNT
		};

		enum GuiInputTextFlagBits
		{
			GuiInputTextFlagCharsDecimalBit = 1 << 0,
			GuiInputTextFlagCharsHexadecimalBit = 1 << 1,
			GuiInputTextFlagCharsUppercaseBit = 1 << 2,
			GuiInputTextFlagCharsNoBlankBit = 1 << 3,
			GuiInputTextFlagAutoSelectAllBit = 1 << 4,
			GuiInputTextFlagEnterReturnsTrueBit = 1 << 5,
			GuiInputTextFlagCallbackCompletionBit = 1 << 6,
			GuiInputTextFlagCallbackHistoryBit = 1 << 7,
			GuiInputTextFlagCallbackAlwaysBit = 1 << 8,
			GuiInputTextFlagCallbackCharFilterBit = 1 << 9,
			GuiInputTextFlagAllowTabInputBit = 1 << 10,
			GuiInputTextFlagCtrlEnterForNewLineBit = 1 << 11,
			GuiInputTextFlagNoHorizontalScrollBit = 1 << 12,
			GuiInputTextFlagAlwaysInsertModeBit = 1 << 13,
			GuiInputTextFlagReadOnlyBit = 1 << 14,
			GuiInputTextFlagPasswordBit = 1 << 15,
		};

		typedef std::uint32_t GuiInputTextFlags;

		enum GuiWindowFlagBits
		{
			GuiWindowFlagNoTitleBarBit = 1 << 0,
			GuiWindowFlagNoResizeBit = 1 << 1,
			GuiWindowFlagNoMoveBit = 1 << 2,
			GuiWindowFlagNoScrollbarBit = 1 << 3,
			GuiWindowFlagNoScrollWithMouseBit = 1 << 4,
			GuiWindowFlagNoCollapseBit = 1 << 5,
			GuiWindowFlagAlwaysAutoResizeBit = 1 << 6,
			GuiWindowFlagShowBordersBit = 1 << 7,
			GuiWindowFlagNoSavedSettingsBit = 1 << 8,
			GuiWindowFlagNoInputsBit = 1 << 9,
			GuiWindowFlagMenuBarBit = 1 << 10,
			GuiWindowFlagHorizontalScrollbarBit = 1 << 11,
			GuiWindowFlagNoFocusOnAppearingBit = 1 << 12,
			GuiWindowFlagNoBringToFrontOnFocusBit = 1 << 13,
			GuiWindowFlagAlwaysVerticalScrollbarBit = 1 << 14,
			GuiWindowFlagAlwaysHorizontalScrollbarBit = 1 << 15,
			GuiWindowFlagAlwaysUseWindowPaddingBit = 1 << 16
		};

		typedef std::uint32_t GuiWindowFlags;

		enum GuiAlignFlagBits
		{
			GuiAlignFlagLeftBit = 1 << 0,
			GuiAlignFlagCenterBit = 1 << 1,
			GuiAlignFlagRightBit = 1 << 2,
			GuiAlignFlagTopBit = 1 << 3,
			GuiAlignFlagVCenterBit = 1 << 4,
			GuiAlignFlagDefaultBit = GuiAlignFlagLeftBit | GuiAlignFlagTopBit
		};

		typedef std::uint32_t GuiAlignFlags;

		enum GuiSetCondFlagBits
		{
			GuiSetCondFlagAlwaysBit = 1 << 0,
			GuiSetCondFlagOnceBit = 1 << 1,
			GuiSetCondFlagFirstUseEverBit = 1 << 2,
			GuiSetCondFlagAppearingBit = 1 << 3
		};

		typedef std::uint32_t GuiSetCondFlags;

		enum GuiTreeNodeFlagBits
		{
			GuiTreeNodeFlagSelectedBit = 1 << 0,
			GuiTreeNodeFlagFramedBit = 1 << 1,
			GuiTreeNodeFlagAllowOverlapModeBit = 1 << 2,
			GuiTreeNodeFlagNoTreePushOnOpenBit = 1 << 3,
			GuiTreeNodeFlagNoAutoOpenOnLogBit = 1 << 4,
			GuiTreeNodeFlagDefaultOpenBit = 1 << 5,
			GuiTreeNodeFlagOpenOnDoubleClickBit = 1 << 6,
			GuiTreeNodeFlagOpenOnArrowBit = 1 << 7,
			GuiTreeNodeFlagLeafBit = 1 << 8,
			GuiTreeNodeFlagBulletBit = 1 << 9,
			GuiTreeNodeFlagCollapsingHeaderBit = GuiTreeNodeFlagFramedBit | GuiTreeNodeFlagNoAutoOpenOnLogBit
		};

		typedef std::uint32_t GuiTreeNodeFlags;

		enum GuiSelectableFlagBits
		{
			GuiSelectableFlagDontClosePopupsBit = 1 << 0,
			GuiSelectableFlagSpanAllColumnsBit = 1 << 1,
			GuiSelectableFlagAllowDoubleClickBit = 1 << 2
		};

		typedef std::uint32_t GuiSelectableFlags;

		enum class GuiMouseCursor
		{
			GuiMouseCursorArrow = 0,
			GuiMouseCursorTextInput,         // When hovering over InputText, etc.
			GuiMouseCursorMove,              // Unused
			GuiMouseCursorResizeNS,          // Unused
			GuiMouseCursorResizeEW,          // When hovering over a column
			GuiMouseCursorResizeNESW,        // Unused
			GuiMouseCursorResizeNWSE,        // When hovering over the bottom-right corner of a window
			GuiMouseCursorCount_
		};

		enum class GuiColorEditMode
		{
			GuiColorEditModeUserSelect = -2,
			GuiColorEditModeUserSelectShowButton = -1,
			GuiColorEditModeRGB = 0,
			GuiColorEditModeHSV = 1,
			GuiColorEditModeHEX = 2
		};

		enum class GuiStyleVar
		{
			GuiStyleVarAlpha,               // float
			GuiStyleVarWindowPadding,       // float2
			GuiStyleVarWindowRounding,      // float
			GuiStyleVarWindowMinSize,       // float2
			GuiStyleVarChildWindowRounding, // float
			GuiStyleVarFramePadding,        // float2
			GuiStyleVarFrameRounding,       // float
			GuiStyleVarItemSpacing,         // float2
			GuiStyleVarItemInnerSpacing,    // float2
			GuiStyleVarIndentSpacing,       // float
			GuiStyleVarGrabMinSize,         // float
			GuiStyleVarButtonTextAlign
		};

		struct OCTOON_EXPORT GuiStyle
		{
			float       Alpha;
			float2      WindowPadding;
			float2      WindowMinSize;
			float       WindowRounding;
			float2	    WindowTitleAlign;
			float       ChildWindowRounding;
			float2      FramePadding;
			float       FrameRounding;
			float2      ItemSpacing;
			float2      ItemInnerSpacing;
			float2      TouchExtraPadding;
			float       IndentSpacing;
			float       ColumnsMinSpacing;
			float       ScrollbarSize;
			float       ScrollbarRounding;
			float       GrabMinSize;
			float       GrabRounding;
			float2      ButtonTextAlign;
			float2      DisplayWindowPadding;
			float2      DisplaySafeAreaPadding;
			bool        AntiAliasedLines;
			bool        AntiAliasedShapes;
			float       CurveTessellationTol;
			float4      Colors[(int)GuiCol::GuiColCOUNT];

			GuiStyle() noexcept;
		};

		typedef void* GuiTextureID;
		typedef std::uint32_t GuiID;

		void new_frame() noexcept;
		void render() noexcept;
		void shutdown() noexcept;

		void show_user_guide() noexcept;
		void show_style_editor(GuiStyle* style = nullptr) noexcept;
		void show_test_window(bool* isOpened = nullptr) noexcept;
		void show_metrics_window(bool* isOpened = nullptr) noexcept;

		bool begin(const char* name, bool* isOpened = nullptr, GuiWindowFlags flags = 0) noexcept;
		bool begin(const char* name, bool* isOpened, const float2& size_on_first_use, float bg_alpha = -1.0f, GuiWindowFlags flags = 0) noexcept;

		bool begin_child(const char* str_id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		bool begin_child(GuiID id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		bool begin_child_frame(GuiID id, const float2& size, GuiWindowFlags extraFlags = 0) noexcept;

		void end() noexcept;
		void end_child() noexcept;
		void end_child_frame() noexcept;

		void begin_group() noexcept;
		void end_group() noexcept;

		bool begin_main_menu_bar() noexcept;
		bool begin_menu_bar() noexcept;
		bool begin_menu(const char* label, bool enabled = true) noexcept;

		bool menu_item(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) noexcept;
		bool menu_item(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) noexcept;

		void end_main_menu_bar() noexcept;
		void end_menu_bar() noexcept;
		void end_menu() noexcept;

		void begin_tooltip() noexcept;

		void set_tooltip(const char* fmt, ...) noexcept;
		void set_tooltipV(const char* fmt, va_list args) noexcept;

		void end_tooltip() noexcept;

		bool begin_popup(const char* str_id) noexcept;
		bool begin_popup_modal(const char* name, bool* isOpened = nullptr, GuiWindowFlags extraFlags = 0) noexcept;
		bool begin_popup_context_item(const char* str_id, int mouse_button = 1) noexcept;
		bool begin_popup_context_window(bool also_over_items = true, const char* str_id = nullptr, int mouse_button = 1) noexcept;
		bool begin_popup_context_void(const char* str_id = nullptr, int mouse_button = 1) noexcept;

		void open_popup(const char* str_id) noexcept;
		void close_current_popup() noexcept;

		void end_popup() noexcept;

		void set_style(const GuiStyle& newStyle) noexcept;
		const GuiStyle& get_style_default() noexcept;

		void set_window_pos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		void set_window_size(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		void set_window_collapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		void set_window_focus() noexcept;
		void set_window_pos(const char* name, const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		void set_window_size(const char* name, const float2& size, GuiSetCondFlags cond = 0) noexcept;
		void set_window_collapsed(const char* name, bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		void set_window_focus(const char* name) noexcept;
		void set_window_fontScale(float scale) noexcept;

		void set_next_window_pos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		void set_next_window_pos_center(GuiSetCondFlags cond = 0) noexcept;
		void set_next_window_size(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		void set_next_window_size_constraints(const float2& size_min, const float2& size_max) noexcept;
		void set_next_window_content_size(const float2& size) noexcept;
		void set_next_window_content_width(float width) noexcept;
		void set_next_window_collapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		void set_next_window_focus() noexcept;

		void set_cursor_pos_x(float x) noexcept;
		void set_cursor_pos_y(float y) noexcept;
		void set_cursor_pos(const float2& local_pos) noexcept;
		void set_cursor_screen_pos(const float2& pos) noexcept;
		void set_cursor_icon(GuiMouseCursor type) noexcept;

		void set_keyboard_focus_here(int offset = 0) noexcept;

		bool is_window_hovered() noexcept;
		bool is_window_focused() noexcept;
		bool is_window_collapsed() noexcept;

		bool is_root_window_focused() noexcept;
		bool is_root_window_or_any_child_focused() noexcept;
		bool is_root_window_or_any_child_hovered() noexcept;

		bool is_rect_visible(const float2& size) noexcept;

		bool is_pos_hovering_any_window(const float2& pos) noexcept;

		bool is_key_down(int key_index) noexcept;
		bool is_key_pressed(int key_index, bool repeat = true) noexcept;
		bool is_key_released(int key_index) noexcept;

		bool is_mouse_down(int button) noexcept;
		bool is_mouse_clicked(int button, bool repeat = false) noexcept;
		bool is_mouse_double_clicked(int button) noexcept;
		bool is_mouse_released(int button) noexcept;
		bool is_mouse_hovering_window() noexcept;
		bool is_mouse_hovering_any_window() noexcept;
		bool is_mouse_hovering_rect(const float2& min, const float2& max, bool clip = true) noexcept;
		bool is_mouse_dragging(int button = 0, float lock_threshold = -1.0f) noexcept;

		float get_content_region_avail_width() noexcept;

		float get_window_width() noexcept;
		float get_window_height() noexcept;
		float get_window_contentRegionWidth() noexcept;

		float get_cursor_pos_x() noexcept;
		float get_cursor_pos_y() noexcept;

		float2 get_cursor_pos() noexcept;
		float2 get_cursor_start_pos() noexcept;
		float2 get_cursor_screen_pos() noexcept;

		float2 get_mouse_pos() noexcept;
		float2 get_mouse_pos_on_opening_current_popup() noexcept;
		float2 get_mouse_drag_delta(int button = 0, float lock_threshold = -1.0f) noexcept;

		float2 get_window_pos() noexcept;
		float2 get_window_size() noexcept;
		float2 get_window_content_region_min() noexcept;
		float2 get_window_content_region_max() noexcept;

		float2 get_content_region_max() noexcept;
		float2 get_content_region_avail() noexcept;

		float2 get_display_size() noexcept;

		void reset_mouse_drag_delta(int button = 0) noexcept;

		void capture_keyboard_from_app(bool capture = true) noexcept;
		void capture_mouse_from_app(bool capture = true) noexcept;

		float get_scroll_x() noexcept;
		float get_scroll_y() noexcept;
		float get_scroll_max_x() noexcept;
		float get_scroll_max_y() noexcept;

		void  set_scroll_x(float scroll_x) noexcept;
		void  set_scroll_y(float scroll_y) noexcept;
		void  set_scroll_here(float center_y_ratio = 0.5f) noexcept;
		void  set_scroll_from_pos_y(float pos_y, float center_y_ratio = 0.5f) noexcept;

		void          push_style_color(GuiCol idx, const float4& col) noexcept;
		void          pop_style_color(int count = 1) noexcept;
		void          push_style_var(GuiStyleVar idx, float val) noexcept;
		void          push_style_var(GuiStyleVar idx, const float2& val) noexcept;
		void          pop_style_var(int count = 1) noexcept;
		float         get_font_size() noexcept;
		float2        get_font_tex_uv_white_pixel() noexcept;
		std::uint32_t get_color_u32(GuiCol idx, float alpha_mul = 1.0f) noexcept;
		std::uint32_t get_color_u32(const float4& col) noexcept;

		void  push_item_width(float item_width) noexcept;
		void  pop_item_width() noexcept;
		float calc_item_width() noexcept;
		void  push_text_wrap_pos(float wrap_pos_x = 0.0f) noexcept;
		void  pop_text_wrap_pos() noexcept;
		void  push_allow_keyboard_focus(bool v) noexcept;
		void  pop_allow_keyboard_focus() noexcept;
		void  push_button_repeat(bool repeat) noexcept;
		void  pop_button_repeat() noexcept;

		void push_clip_rect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept;
		void pop_clip_rect() noexcept;

		void separator() noexcept;
		void same_line(float pos_x = 0.0f, float spacing_w = -1.0f) noexcept;
		void new_line() noexcept;
		void spacing() noexcept;
		void dummy(const float2& size) noexcept;
		void indent(float indent_w = 0.0f) noexcept;
		void unindent(float indent_w = 0.0f) noexcept;

		void align_first_text_height_to_widgets() noexcept;
		float get_text_line_height() noexcept;
		float get_text_line_height_with_spacing() noexcept;
		float get_items_line_height_with_spacing() noexcept;

		void columns(int count = 1, const char* id = nullptr, bool border = true) noexcept;

		void push_id(const char* str_id_begin, const char* str_id_end) noexcept;
		void push_id(const void* ptr_id) noexcept;
		void push_id(int int_id) noexcept;
		void pop_id() noexcept;

		GuiID get_id(const char* str_id) noexcept;
		GuiID get_id(const char* str_id_begin, const char* str_id_end) noexcept;
		GuiID get_id(const void* ptr_id) noexcept;

		void text(const char* fmt, ...) noexcept;
		void text_v(const char* fmt, va_list args) noexcept;
		void text_colored(const float4& col, const char* fmt, ...) noexcept;
		void text_colored_v(const float4& col, const char* fmt, va_list args) noexcept;
		void text_disabled(const char* fmt, ...) noexcept;
		void text_disabled_v(const char* fmt, va_list args) noexcept;
		void text_wrapped(const char* fmt, ...) noexcept;
		void text_wrapped_v(const char* fmt, va_list args) noexcept;
		void text_unformatted(const char* text, const char* text_end = nullptr) noexcept;

		void label_text(const char* label, const char* fmt, ...) noexcept;
		void label_text_v(const char* label, const char* fmt, va_list args) noexcept;
		void label_text_ex(const char* label, const char* fmt, ...);
		void label_text_ex_v(const char* label, const char* fmt, va_list args);

		void bullet() noexcept;
		void bullet_text(const char* fmt, ...) noexcept;
		void bullet_text_v(const char* fmt, va_list args) noexcept;

		bool button(const char* label, const float2& size = float2::Zero) noexcept;
		bool small_button(const char* label) noexcept;
		bool invisible_button(const char* str_id, const float2& size) noexcept;

		void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), const float4& tint_col = float4(1, 1, 1, 1), const float4& border_col = float4(0, 0, 0, 0)) noexcept;
		bool image_button(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), int frame_padding = -1, const float4& bg_col = float4(0, 0, 0, 0), const float4& tint_col = float4(1, 1, 1, 1)) noexcept;
		bool image_button_ex(GuiTextureID texture, const float2& size = float2(24, 24), const char* tooltip = nullptr, bool selected = false, bool enabled = true);
		bool image_button_ex(GuiTextureID texture, const float2& size, bool enabled = true, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One);
		bool image_button_and_label(const char* label, GuiTextureID texture, const float2& size, bool showLabel = true, bool selected = false, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One);
		bool image_button_with_aspect_and_label(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0);
		void image_with_aspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0 = float2(0, 0), const float2& uv1 = float2(1, 1), const float4& tint_col = float4::One, const float4& border_col = float4::Zero);

		bool checkbox(const char* label, bool* v) noexcept;
		bool checkbox_flags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept;

		bool radio_button(const char* label, bool active) noexcept;
		bool radio_button(const char* label, int* v, int v_button) noexcept;

		bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		bool combo_with_revert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items = -1) noexcept;
		bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1) noexcept;
		bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;

		bool color_button(const float4& col, bool small_height = false, bool outline_border = true) noexcept;
		bool color_edit3(const char* label, float col[3]) noexcept;
		bool color_edit4(const char* label, float col[4], bool show_alpha = true) noexcept;
		void color_edit_mode(GuiColorEditMode mode) noexcept;

		void plot_lines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		void plot_lines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;
		void plot_histogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		void plot_histogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;

		void progress_bar(float fraction, const float2& size_arg = float2(-1, 0), const char* overlay = nullptr) noexcept;

		bool drag_float(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool drag_float2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool drag_float3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool drag_float4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool drag_float_range2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = nullptr, float power = 1.0f) noexcept;
		bool drag_int(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		bool drag_int2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		bool drag_int3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		bool drag_int4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		bool drag_int_range2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = nullptr) noexcept;
		bool drag_floatn_ex(const char* labels[], float* v, int components, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
		bool drag_intn_ex(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
		bool drag_uIntn_ex(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* display_format = "%.0f");

		bool input_text(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		bool input_text_multiline(const char* label, char* buf, size_t buf_size, const float2& size = float2::Zero, GuiInputTextFlags flags = 0) noexcept;
		bool input_float(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_float2(const char* label, float v[2], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_float3(const char* label, float v[3], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_float4(const char* label, float v[4], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_int(const char* label, int* v, int step = 1, int step_fast = 100, GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_int2(const char* label, int v[2], GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_int3(const char* label, int v[3], GuiInputTextFlags extraFlags = 0) noexcept;
		bool input_int4(const char* label, int v[4], GuiInputTextFlags extraFlags = 0) noexcept;

		bool slider_float(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float2(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float3(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float4(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_floatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float_with_revert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float2_with_revert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float3_with_revert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		bool slider_float4_with_revert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;

		bool slider_angle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f) noexcept;
		bool slider_int(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		bool slider_int2(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		bool slider_int3(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		bool slider_int4(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		bool slider_intv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		bool slider_int_with_revert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format = "%.0f") noexcept;
		bool slider_int2_with_revert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format = "%.0f") noexcept;
		bool slider_int3_with_revert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format = "%.0f") noexcept;
		bool slider_int4_with_revert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format = "%.0f") noexcept;

		bool  tree_node(const char* label) noexcept;
		bool  tree_node(const char* str_id, const char* fmt, ...) noexcept;
		bool  tree_node(const void* ptr_id, const char* fmt, ...) noexcept;
		bool  tree_node_v(const char* str_id, const char* fmt, va_list args) noexcept;
		bool  tree_node_v(const void* ptr_id, const char* fmt, va_list args) noexcept;
		bool  tree_node_ex(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		bool  tree_node_ex(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		bool  tree_node_ex(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		bool  tree_node_ex_v(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		bool  tree_node_ex_v(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		void  tree_push(const char* str_id = nullptr) noexcept;
		void  tree_push(const void* ptr_id = nullptr) noexcept;
		void  tree_pop() noexcept;
		void  tree_advance_to_label_pos() noexcept;
		float get_tree_node_to_label_spacing() noexcept;
		void  set_next_tree_node_open(bool is__open, GuiSetCondFlags cond = 0) noexcept;
		bool  collapsing_header(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		bool  collapsing_header(const char* label, bool* isOpened, GuiTreeNodeFlags flags = 0) noexcept;

		bool selectable(const char* label, bool selected = false, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		bool selectable(const char* label, bool* p_selected, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		bool list_box(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		bool list_box(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;
		bool list_box_header(const char* label, const float2& size = float2::Zero) noexcept;
		bool list_box_header(const char* label, int items_count, int height_in_items = -1) noexcept;
		void list_box_footer() noexcept;

		void value(const char* prefix, bool b) noexcept;
		void value(const char* prefix, int v) noexcept;
		void value(const char* prefix, unsigned int v) noexcept;
		void value(const char* prefix, float v, const char* float_format = nullptr) noexcept;
		void value_color(const char* prefix, const float4& v) noexcept;
		void value_color(const char* prefix, std::uint32_t v) noexcept;

		float2 get_item_rect_min() noexcept;
		float2 get_item_rect_max() noexcept;
		float2 get_item_rect_size() noexcept;
		void set_item_allow_overlap() noexcept;

		bool is_item_hovered() noexcept;
		bool is_item_hovered_rect() noexcept;
		bool is_item_active() noexcept;
		bool is_item_clicked(int mouse_button = 0) noexcept;
		bool is_item_visible() noexcept;

		bool is_any_item_active() noexcept;
		bool is_any_item_hovered() noexcept;

		float get_time() noexcept;
		int get_frame_count() noexcept;
		const char* get_style_col_name(GuiCol idx) noexcept;
		float2 calc_item_rect_closest_point(const float2& pos, bool on_edge = false, float outward = +0.0f) noexcept;
		float2 calc_text_size(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) noexcept;
		void calc_list_clipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept;

		float4 colorConvertU32ToFloat4(std::uint32_t in) noexcept;
		std::uint32_t colorConvertFloat4ToU32(const float4& in) noexcept;
		void color_convertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept;
		void color_convertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept;

		bool color_picker3(const char* label, float col[3], const float2& size = float2(160.f, 150.f), float hueSize = 12.0f, float crossHairSize = 7.0f) noexcept;
		bool color_picker3_with_revert(const char* label, const char* name, float col[3], const float default[3], const float2& size = float2(160.f, 150.f), float hueSize = 12.0f, float crossHairSize = 7.0f) noexcept;

		void help_marker(const char* text, const char* desc) noexcept;

		void root_dock(const float2& pos, const float2& size);
		bool begin_dock(const char* label, bool* opened = nullptr, GuiWindowFlags extra_flags = 0, const float2& default_size = float2(-1, -1));
		void end_dock();
		void set_dock_active();

		void render_frame_ex(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness = 1.0f);
		bool begin_toolbar(const char* str_id, const float2& screen_pos, const float2& size);
		void end_toolbar();
		bool toolbar_button(GuiTextureID texture, const char* tooltip, bool selected = false, bool enabled = true);
	}
}

#endif