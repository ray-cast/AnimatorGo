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

		enum class GuiCol
		{
			Text,
			TextDisabled,
			WindowBg,              // Background of normal windows
			ChildBg,               // Background of child windows
			PopupBg,               // Background of popups, menus, tooltips windows
			Border,
			BorderShadow,
			FrameBg,               // Background of checkbox, radio button, plot, slider, text input
			FrameBgHovered,
			FrameBgActive,
			TitleBg,
			TitleBgActive,
			TitleBgCollapsed,
			MenuBarBg,
			ScrollbarBg,
			ScrollbarGrab,
			ScrollbarGrabHovered,
			ScrollbarGrabActive,
			CheckMark,
			SliderGrab,
			SliderGrabActive,
			Button,
			ButtonHovered,
			ButtonActive,
			Header,
			HeaderHovered,
			HeaderActive,
			Separator,
			SeparatorHovered,
			SeparatorActive,
			ResizeGrip,
			ResizeGripHovered,
			ResizeGripActive,
			PlotLines,
			PlotLinesHovered,
			PlotHistogram,
			PlotHistogramHovered,
			TextSelectedBg,
			ModalWindowDarkening,  // darken/colorize entire screen behind a modal window, when one is active
			DragDropTarget,
			NavHighlight,          // gamepad/keyboard: current highlighted item
			NavWindowingHighlight, // gamepad/keyboard: when holding NavMenu to focus/move/resize windows
			Count_
		};

		struct GuiInputTextFlagBits
		{
			enum Flags
			{
				CharsDecimalBit = 1 << 0,
				CharsHexadecimalBit = 1 << 1,
				CharsUppercaseBit = 1 << 2,
				CharsNoBlankBit = 1 << 3,
				AutoSelectAllBit = 1 << 4,
				EnterReturnsTrueBit = 1 << 5,
				CallbackCompletionBit = 1 << 6,
				CallbackHistoryBit = 1 << 7,
				CallbackAlwaysBit = 1 << 8,
				CallbackCharFilterBit = 1 << 9,
				AllowTabInputBit = 1 << 10,
				CtrlEnterForNewLineBit = 1 << 11,
				NoHorizontalScrollBit = 1 << 12,
				AlwaysInsertModeBit = 1 << 13,
				ReadOnlyBit = 1 << 14,
				PasswordBit = 1 << 15
			};
		};

		typedef std::uint32_t GuiInputTextFlags;

		struct GuiWindowFlagBits
		{
			enum Flags
			{
				NoTitleBarBit = 1 << 0,
				NoResizeBit = 1 << 1,
				NoMoveBit = 1 << 2,
				NoScrollbarBit = 1 << 3,
				NoScrollWithMouseBit = 1 << 4,
				NoCollapseBit = 1 << 5,
				AlwaysAutoResizeBit = 1 << 6,
				ShowBordersBit = 1 << 7,
				NoSavedSettingsBit = 1 << 8,
				NoInputsBit = 1 << 9,
				MenuBarBit = 1 << 10,
				HorizontalScrollbarBit = 1 << 11,
				NoFocusOnAppearingBit = 1 << 12,
				NoBringToFrontOnFocusBit = 1 << 13,
				AlwaysVerticalScrollbarBit = 1 << 14,
				AlwaysHorizontalScrollbarBit = 1 << 15,
				AlwaysUseWindowPaddingBit = 1 << 16
			};
		};

		typedef std::uint32_t GuiWindowFlags;

		struct GuiAlignFlagBits
		{
			enum Flags
			{
				LeftBit = 1 << 0,
				CenterBit = 1 << 1,
				RightBit = 1 << 2,
				TopBit = 1 << 3,
				VCenterBit = 1 << 4,
				DefaultBit = LeftBit | TopBit
			};
		};

		typedef std::uint32_t GuiAlignFlags;

		struct GuiSetCondFlagBits
		{
			enum Flags
			{
				AlwaysBit = 1 << 0,
				OnceBit = 1 << 1,
				FirstUseEverBit = 1 << 2,
				AppearingBit = 1 << 3
			};
		};

		typedef std::uint32_t GuiSetCondFlags;

		struct GuiTreeNodeFlagBits
		{
			enum Flags
			{
				SelectedBit = 1 << 0,
				FramedBit = 1 << 1,
				AllowOverlapModeBit = 1 << 2,
				NoTreePushOnOpenBit = 1 << 3,
				NoAutoOpenOnLogBit = 1 << 4,
				DefaultOpenBit = 1 << 5,
				OpenOnDoubleClickBit = 1 << 6,
				OpenOnArrowBit = 1 << 7,
				LeafBit = 1 << 8,
				BulletBit = 1 << 9,
				CollapsingHeaderBit = FramedBit | NoAutoOpenOnLogBit
			};
		};

		typedef std::uint32_t GuiTreeNodeFlags;

		struct GuiSelectableFlagBits
		{
			enum Flags
			{
				DontClosePopupsBit = 1 << 0,
				SpanAllColumnsBit = 1 << 1,
				AllowDoubleClickBit = 1 << 2
			};
		};

		typedef std::uint32_t GuiSelectableFlags;

		enum class GuiMouseCursor
		{
			Arrow = 0,
			TextInput,         // When hovering over InputText, etc.
			Move,              // Unused
			ResizeNS,          // Unused
			ResizeEW,          // When hovering over a column
			ResizeNESW,        // Unused
			ResizeNWSE,        // When hovering over the bottom-right corner of a window
			Count_
		};

		enum class GuiColorEditMode
		{
			UserSelect = -2,
			UserSelectShowButton = -1,
			RGB = 0,
			HSV = 1,
			HEX = 2
		};

		struct GuiColorEditFlagBits
		{
			enum Flags
			{
				NoAlpha = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (read 3 components from the input pointer).
				NoPicker = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
				NoOptions = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
				NoSmallPreview = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
				NoInputs = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
				NoTooltip = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
				NoLabel = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
				NoSidePreview = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.

				// User Options (right-click on widget to change some of them). You can set application defaults using SetColorEditOptions(). The idea is that you probably don't want to override them in most of your calls, let the user choose and/or call SetColorEditOptions() during startup.
				AlphaBar = 1 << 9,   //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
				AlphaPreview = 1 << 10,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
				AlphaPreviewHalf = 1 << 11,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
				HDR = 1 << 12,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use ImGuiColorEditFlags_Float flag as well).
				RGB = 1 << 13,  // [Inputs]     // ColorEdit: choose one among RGB/HSV/HEX. ColorPicker: choose any combination using RGB/HSV/HEX.
				HSV = 1 << 14,  // [Inputs]     // "
				HEX = 1 << 15,  // [Inputs]     // "
				Uint8 = 1 << 16,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
				Float = 1 << 17,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
				PickerHueBar = 1 << 18,  // [PickerMode] // ColorPicker: bar for Hue, rectangle for Sat/Value.
				PickerHueWheel = 1 << 19,  // [PickerMode] // ColorPicker: wheel for Hue, triangle for Sat/Value.

				// [Internal] Masks
				InputsMask = RGB | HSV | HEX,
				DataTypeMask = Uint8 | Float,
				PickerMask = PickerHueWheel | PickerHueBar,
				OptionsDefault = Uint8 | RGB | PickerHueBar    // Change application default using SetColorEditOptions()
			};
		};

		typedef std::uint32_t GuiColorEditFlags;

		enum class GuiStyleVar
		{
			Alpha,               // float
			WindowPadding,       // float2
			WindowRounding,      // float
			WindowMinSize,       // float2
			ChildWindowRounding, // float
			FramePadding,        // float2
			FrameRounding,       // float
			ItemSpacing,         // float2
			ItemInnerSpacing,    // float2
			IndentSpacing,       // float
			GrabMinSize,         // float
			ButtonTextAlign
		};

		enum class GuiDir
		{
			None = -1,
			Left = 0,
			Right = 1,
			Up = 2,
			Down = 3,
		};

		struct OCTOON_EXPORT GuiStyle
		{
		  	float       Alpha;                      // Global alpha applies to everything in ImGui.
		    float2      WindowPadding;              // Padding within a window.
		    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
		    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		    float2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
		    float2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
		    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
		    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
		    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		    float2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
		    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
		    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
		    float2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
		    float2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
		    float2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
		    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
		    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns.
		    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
		    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
		    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
		    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
		    float2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f,0.5f) for horizontally+vertically centered.
		    float2      DisplayWindowPadding;       // Window positions are clamped to be visible within the display area by at least this amount. Only covers regular windows.
		    float2      DisplaySafeAreaPadding;     // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
		    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
		    bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
		    bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
		    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
			float4      Colors[(int)GuiCol::Count_];

			GuiStyle() noexcept;
		};

		typedef void* GuiTextureID;
		typedef std::uint32_t GuiID;

		OCTOON_EXPORT void new_frame() noexcept;

		OCTOON_EXPORT void show_user_guide() noexcept;
		OCTOON_EXPORT void show_style_editor(GuiStyle* style = nullptr) noexcept;
		OCTOON_EXPORT void show_test_window() noexcept;
		OCTOON_EXPORT void show_metrics_window(bool* isOpened = nullptr) noexcept;

		OCTOON_EXPORT bool begin(const char* name, bool* isOpened = nullptr, GuiWindowFlags flags = 0) noexcept;
		OCTOON_EXPORT bool begin(const char* name, bool* isOpened, const float2& size_on_first_use, float bg_alpha = -1.0f, GuiWindowFlags flags = 0) noexcept;

		OCTOON_EXPORT bool begin_child(const char* str_id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool begin_child(GuiID id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool begin_child_frame(GuiID id, const float2& size, GuiWindowFlags extraFlags = 0) noexcept;

		OCTOON_EXPORT void end() noexcept;
		OCTOON_EXPORT void end_child() noexcept;
		OCTOON_EXPORT void end_child_frame() noexcept;

		OCTOON_EXPORT void begin_group() noexcept;
		OCTOON_EXPORT void end_group() noexcept;

		OCTOON_EXPORT bool begin_main_menu_bar() noexcept;
		OCTOON_EXPORT bool begin_menu_bar() noexcept;
		OCTOON_EXPORT bool begin_menu(const char* label, bool enabled = true) noexcept;

		OCTOON_EXPORT bool menu_item(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) noexcept;
		OCTOON_EXPORT bool menu_item(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) noexcept;

		OCTOON_EXPORT void end_main_menu_bar() noexcept;
		OCTOON_EXPORT void end_menu_bar() noexcept;
		OCTOON_EXPORT void end_menu() noexcept;

		OCTOON_EXPORT void begin_tooltip() noexcept;

		OCTOON_EXPORT void set_tooltip(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void set_tooltipV(const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT void end_tooltip() noexcept;

		OCTOON_EXPORT bool begin_popup(const char* str_id) noexcept;
		OCTOON_EXPORT bool begin_popup_modal(const char* name, bool* isOpened = nullptr, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool begin_popup_context_item(const char* str_id, int mouse_button = 1) noexcept;
		OCTOON_EXPORT bool begin_popup_context_window(const char* str_id = nullptr, int mouse_button = 1, bool also_over_items = true) noexcept;
		OCTOON_EXPORT bool begin_popup_context_void(const char* str_id = nullptr, int mouse_button = 1) noexcept;

		OCTOON_EXPORT void open_popup(const char* str_id) noexcept;
		OCTOON_EXPORT void close_current_popup() noexcept;

		OCTOON_EXPORT void end_popup() noexcept;

		OCTOON_EXPORT void set_style(const GuiStyle& newStyle) noexcept;
		OCTOON_EXPORT const GuiStyle& get_style_default() noexcept;

		OCTOON_EXPORT void set_window_pos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_size(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_collapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_focus() noexcept;
		OCTOON_EXPORT void set_window_pos(const char* name, const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_size(const char* name, const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_collapsed(const char* name, bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_window_focus(const char* name) noexcept;
		OCTOON_EXPORT void set_window_fontScale(float scale) noexcept;

		OCTOON_EXPORT void set_next_window_pos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_next_window_pos_center(GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_next_window_size(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_next_window_size_constraints(const float2& size_min, const float2& size_max) noexcept;
		OCTOON_EXPORT void set_next_window_content_size(const float2& size) noexcept;
		OCTOON_EXPORT void set_next_window_content_width(float width) noexcept;
		OCTOON_EXPORT void set_next_window_collapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void set_next_window_focus() noexcept;

		OCTOON_EXPORT void set_cursor_pos_x(float x) noexcept;
		OCTOON_EXPORT void set_cursor_pos_y(float y) noexcept;
		OCTOON_EXPORT void set_cursor_pos(const float2& local_pos) noexcept;
		OCTOON_EXPORT void set_cursor_screen_pos(const float2& pos) noexcept;
		OCTOON_EXPORT void set_cursor_icon(GuiMouseCursor type) noexcept;

		OCTOON_EXPORT void set_keyboard_focus_here(int offset = 0) noexcept;

		OCTOON_EXPORT bool is_window_hovered() noexcept;
		OCTOON_EXPORT bool is_window_focused() noexcept;
		OCTOON_EXPORT bool is_window_collapsed() noexcept;

		OCTOON_EXPORT bool is_root_window_focused() noexcept;
		OCTOON_EXPORT bool is_root_window_or_any_child_focused() noexcept;
		OCTOON_EXPORT bool is_root_window_or_any_child_hovered() noexcept;

		OCTOON_EXPORT bool is_rect_visible(const float2& size) noexcept;

		OCTOON_EXPORT bool is_pos_hovering_any_window(const float2& pos) noexcept;

		OCTOON_EXPORT bool is_key_down(int key_index) noexcept;
		OCTOON_EXPORT bool is_key_pressed(int key_index, bool repeat = true) noexcept;
		OCTOON_EXPORT bool is_key_released(int key_index) noexcept;

		OCTOON_EXPORT bool is_mouse_down(int button) noexcept;
		OCTOON_EXPORT bool is_mouse_clicked(int button, bool repeat = false) noexcept;
		OCTOON_EXPORT bool is_mouse_double_clicked(int button) noexcept;
		OCTOON_EXPORT bool is_mouse_released(int button) noexcept;
		OCTOON_EXPORT bool is_mouse_hovering_window() noexcept;
		OCTOON_EXPORT bool is_mouse_hovering_any_window() noexcept;
		OCTOON_EXPORT bool is_mouse_hovering_rect(const float2& min, const float2& max, bool clip = true) noexcept;
		OCTOON_EXPORT bool is_mouse_dragging(int button = 0, float lock_threshold = -1.0f) noexcept;

		OCTOON_EXPORT float get_content_region_avail_width() noexcept;

		OCTOON_EXPORT float get_window_width() noexcept;
		OCTOON_EXPORT float get_window_height() noexcept;
		OCTOON_EXPORT float get_window_contentRegionWidth() noexcept;

		OCTOON_EXPORT float get_cursor_pos_x() noexcept;
		OCTOON_EXPORT float get_cursor_pos_y() noexcept;

		OCTOON_EXPORT float2 get_cursor_pos() noexcept;
		OCTOON_EXPORT float2 get_cursor_start_pos() noexcept;
		OCTOON_EXPORT float2 get_cursor_screen_pos() noexcept;

		OCTOON_EXPORT float2 get_mouse_pos() noexcept;
		OCTOON_EXPORT float2 get_mouse_pos_on_opening_current_popup() noexcept;
		OCTOON_EXPORT float2 get_mouse_drag_delta(int button = 0, float lock_threshold = -1.0f) noexcept;

		OCTOON_EXPORT float2 get_window_pos() noexcept;
		OCTOON_EXPORT float2 get_window_size() noexcept;
		OCTOON_EXPORT float2 get_window_content_region_min() noexcept;
		OCTOON_EXPORT float2 get_window_content_region_max() noexcept;

		OCTOON_EXPORT float2 get_content_region_max() noexcept;
		OCTOON_EXPORT float2 get_content_region_avail() noexcept;

		OCTOON_EXPORT float2 get_display_size() noexcept;

		OCTOON_EXPORT void reset_mouse_drag_delta(int button = 0) noexcept;

		OCTOON_EXPORT void capture_keyboard_from_app(bool capture = true) noexcept;
		OCTOON_EXPORT void capture_mouse_from_app(bool capture = true) noexcept;

		OCTOON_EXPORT float get_scroll_x() noexcept;
		OCTOON_EXPORT float get_scroll_y() noexcept;
		OCTOON_EXPORT float get_scroll_max_x() noexcept;
		OCTOON_EXPORT float get_scroll_max_y() noexcept;

		OCTOON_EXPORT void  set_scroll_x(float scroll_x) noexcept;
		OCTOON_EXPORT void  set_scroll_y(float scroll_y) noexcept;
		OCTOON_EXPORT void  set_scroll_here(float center_y_ratio = 0.5f) noexcept;
		OCTOON_EXPORT void  set_scroll_from_pos_y(float pos_y, float center_y_ratio = 0.5f) noexcept;

		OCTOON_EXPORT void          push_style_color(GuiCol idx, const float4& col) noexcept;
		OCTOON_EXPORT void          pop_style_color(int count = 1) noexcept;
		OCTOON_EXPORT void          push_style_var(GuiStyleVar idx, float val) noexcept;
		OCTOON_EXPORT void          push_style_var(GuiStyleVar idx, const float2& val) noexcept;
		OCTOON_EXPORT void          pop_style_var(int count = 1) noexcept;
		OCTOON_EXPORT float         get_font_size() noexcept;
		OCTOON_EXPORT float2        get_font_tex_uv_white_pixel() noexcept;
		OCTOON_EXPORT std::uint32_t get_color_u32(GuiCol idx, float alpha_mul = 1.0f) noexcept;
		OCTOON_EXPORT std::uint32_t get_color_u32(const float4& col) noexcept;

		OCTOON_EXPORT void  push_item_width(float item_width) noexcept;
		OCTOON_EXPORT void  pop_item_width() noexcept;
		OCTOON_EXPORT float calc_item_width() noexcept;
		OCTOON_EXPORT void  push_text_wrap_pos(float wrap_pos_x = 0.0f) noexcept;
		OCTOON_EXPORT void  pop_text_wrap_pos() noexcept;
		OCTOON_EXPORT void  push_allow_keyboard_focus(bool v) noexcept;
		OCTOON_EXPORT void  pop_allow_keyboard_focus() noexcept;
		OCTOON_EXPORT void  push_button_repeat(bool repeat) noexcept;
		OCTOON_EXPORT void  pop_button_repeat() noexcept;

		OCTOON_EXPORT void push_clip_rect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept;
		OCTOON_EXPORT void pop_clip_rect() noexcept;

		OCTOON_EXPORT void separator() noexcept;
		OCTOON_EXPORT void same_line(float pos_x = 0.0f, float spacing_w = -1.0f) noexcept;
		OCTOON_EXPORT void new_line() noexcept;
		OCTOON_EXPORT void spacing() noexcept;
		OCTOON_EXPORT void dummy(const float2& size) noexcept;
		OCTOON_EXPORT void indent(float indent_w = 0.0f) noexcept;
		OCTOON_EXPORT void unindent(float indent_w = 0.0f) noexcept;

		OCTOON_EXPORT void align_first_text_height_to_widgets() noexcept;
		OCTOON_EXPORT float get_text_line_height() noexcept;
		OCTOON_EXPORT float get_text_line_height_with_spacing() noexcept;
		OCTOON_EXPORT float get_items_line_height_with_spacing() noexcept;

		OCTOON_EXPORT void columns(int count = 1, const char* id = nullptr, bool border = true) noexcept;

		OCTOON_EXPORT void push_id(const char* str_id_begin, const char* str_id_end) noexcept;
		OCTOON_EXPORT void push_id(const void* ptr_id) noexcept;
		OCTOON_EXPORT void push_id(int int_id) noexcept;
		OCTOON_EXPORT void pop_id() noexcept;

		OCTOON_EXPORT GuiID get_id(const char* str_id) noexcept;
		OCTOON_EXPORT GuiID get_id(const char* str_id_begin, const char* str_id_end) noexcept;
		OCTOON_EXPORT GuiID get_id(const void* ptr_id) noexcept;

		OCTOON_EXPORT void text(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void text_v(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void text_colored(const float4& col, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void text_colored_v(const float4& col, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void text_disabled(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void text_disabled_v(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void text_wrapped(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void text_wrapped_v(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void text_unformatted(const char* text, const char* text_end = nullptr) noexcept;

		OCTOON_EXPORT void label_text(const char* label, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void label_text_v(const char* label, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void label_text_ex(const char* label, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void label_text_ex_v(const char* label, const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT void bullet() noexcept;
		OCTOON_EXPORT void bullet_text(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void bullet_text_v(const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT bool button(const char* label, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool small_button(const char* label) noexcept;
		OCTOON_EXPORT bool invisible_button(const char* str_id, const float2& size) noexcept;
		OCTOON_EXPORT bool arrow_button(const char* str_id, GuiDir dir);

		OCTOON_EXPORT void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), const float4& tint_col = float4(1, 1, 1, 1), const float4& border_col = float4(0, 0, 0, 0)) noexcept;
		OCTOON_EXPORT bool image_button(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), int frame_padding = -1, const float4& bg_col = float4(0, 0, 0, 0), const float4& tint_col = float4(1, 1, 1, 1)) noexcept;
		OCTOON_EXPORT bool image_button_ex(GuiTextureID texture, const float2& size = float2(24, 24), const char* tooltip = nullptr, bool selected = false, bool enabled = true) noexcept;
		OCTOON_EXPORT bool image_button_ex(GuiTextureID texture, const float2& size, bool enabled = true, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One) noexcept;
		OCTOON_EXPORT bool image_button_and_label(const char* label, GuiTextureID texture, const float2& size, bool showLabel = true, bool selected = false, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One) noexcept;
		OCTOON_EXPORT bool image_button_with_aspect_and_label(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT void image_with_aspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0 = float2(0, 0), const float2& uv1 = float2(1, 1), const float4& tint_col = float4::One, const float4& border_col = float4::Zero) noexcept;

		OCTOON_EXPORT bool checkbox(const char* label, bool* v) noexcept;
		OCTOON_EXPORT bool checkbox_flags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept;

		OCTOON_EXPORT bool radio_button(const char* label, bool active) noexcept;
		OCTOON_EXPORT bool radio_button(const char* label, int* v, int v_button) noexcept;

		OCTOON_EXPORT bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool combo_with_revert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;

		OCTOON_EXPORT bool color_button(const char* desc_id, const float4& col, GuiColorEditFlags flags = 0, float2 size = float2(0, 0)) noexcept;
		OCTOON_EXPORT bool color_edit3(const char* label, float col[3]) noexcept;
		OCTOON_EXPORT bool color_edit4(const char* label, float col[4], bool show_alpha = true) noexcept;

		OCTOON_EXPORT void plot_lines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		OCTOON_EXPORT void plot_lines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;
		OCTOON_EXPORT void plot_histogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		OCTOON_EXPORT void plot_histogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;

		OCTOON_EXPORT void progress_bar(float fraction, const float2& size_arg = float2(-1, 0), const char* overlay = nullptr) noexcept;

		OCTOON_EXPORT bool drag_float(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_float2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_float3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_float4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_float_range2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = nullptr, float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_int(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool drag_int2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool drag_int3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool drag_int4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool drag_int_range2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = nullptr) noexcept;
		OCTOON_EXPORT bool drag_floatn_ex(const char* labels[], float* v, int components, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool drag_intn_ex(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool drag_uIntn_ex(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* display_format = "%.0f") noexcept;

		OCTOON_EXPORT bool input_text(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT bool input_text_multiline(const char* label, char* buf, size_t buf_size, const float2& size = float2::Zero, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT bool input_float(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_float2(const char* label, float v[2], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_float3(const char* label, float v[3], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_float4(const char* label, float v[4], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_int(const char* label, int* v, int step = 1, int step_fast = 100, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_int2(const char* label, int v[2], GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_int3(const char* label, int v[3], GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool input_int4(const char* label, int v[4], GuiInputTextFlags extraFlags = 0) noexcept;

		OCTOON_EXPORT bool slider_float(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float2(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float3(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float4(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_floatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float_with_revert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float2_with_revert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float3_with_revert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool slider_float4_with_revert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;

		OCTOON_EXPORT bool slider_angle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f) noexcept;
		OCTOON_EXPORT bool slider_int(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int2(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int3(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int4(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_intv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int_with_revert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int2_with_revert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int3_with_revert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool slider_int4_with_revert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format = "%.0f") noexcept;

		OCTOON_EXPORT bool  tree_node(const char* label) noexcept;
		OCTOON_EXPORT bool  tree_node(const char* str_id, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  tree_node(const void* ptr_id, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  tree_node_v(const char* str_id, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  tree_node_v(const void* ptr_id, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  tree_node_ex(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		OCTOON_EXPORT bool  tree_node_ex(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  tree_node_ex(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  tree_node_ex_v(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  tree_node_ex_v(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void  tree_push(const char* str_id = nullptr) noexcept;
		OCTOON_EXPORT void  tree_push(const void* ptr_id = nullptr) noexcept;
		OCTOON_EXPORT void  tree_pop() noexcept;
		OCTOON_EXPORT void  tree_advance_to_label_pos() noexcept;
		OCTOON_EXPORT float get_tree_node_to_label_spacing() noexcept;
		OCTOON_EXPORT void  set_next_tree_node_open(bool is__open, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT bool  collapsing_header(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		OCTOON_EXPORT bool  collapsing_header(const char* label, bool* isOpened, GuiTreeNodeFlags flags = 0) noexcept;

		OCTOON_EXPORT bool selectable(const char* label, bool selected = false, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool selectable(const char* label, bool* p_selected, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool list_box(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool list_box(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool list_box_header(const char* label, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool list_box_header(const char* label, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT void list_box_footer() noexcept;

		OCTOON_EXPORT void value(const char* prefix, bool b) noexcept;
		OCTOON_EXPORT void value(const char* prefix, int v) noexcept;
		OCTOON_EXPORT void value(const char* prefix, unsigned int v) noexcept;
		OCTOON_EXPORT void value(const char* prefix, float v, const char* float_format = nullptr) noexcept;

		OCTOON_EXPORT float2 get_item_rect_min() noexcept;
		OCTOON_EXPORT float2 get_item_rect_max() noexcept;
		OCTOON_EXPORT float2 get_item_rect_size() noexcept;
		OCTOON_EXPORT void set_item_allow_overlap() noexcept;

		OCTOON_EXPORT bool is_item_hovered() noexcept;
		OCTOON_EXPORT bool is_item_hovered_rect() noexcept;
		OCTOON_EXPORT bool is_item_active() noexcept;
		OCTOON_EXPORT bool is_item_clicked(int mouse_button = 0) noexcept;
		OCTOON_EXPORT bool is_item_visible() noexcept;

		OCTOON_EXPORT bool is_any_item_active() noexcept;
		OCTOON_EXPORT bool is_any_item_hovered() noexcept;

		OCTOON_EXPORT float get_time() noexcept;
		OCTOON_EXPORT int get_frame_count() noexcept;
		OCTOON_EXPORT const char* get_style_col_name(GuiCol idx) noexcept;
		OCTOON_EXPORT float2 calc_item_rect_closest_point(const float2& pos, bool on_edge = false, float outward = +0.0f) noexcept;
		OCTOON_EXPORT float2 calc_text_size(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) noexcept;
		OCTOON_EXPORT void calc_list_clipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept;

		OCTOON_EXPORT float4 colorConvertU32ToFloat4(std::uint32_t in) noexcept;
		OCTOON_EXPORT std::uint32_t colorConvertFloat4ToU32(const float4& in) noexcept;
		OCTOON_EXPORT void color_convertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept;
		OCTOON_EXPORT void color_convertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept;

		OCTOON_EXPORT bool color_picker3(const char* label, float col[3], GuiColorEditFlags flags = 0);
		OCTOON_EXPORT bool color_picker4(const char* label, float col[4], GuiColorEditFlags flags = 0, const float* ref_col = NULL);

		OCTOON_EXPORT void help_marker(const char* text, const char* desc) noexcept;

		OCTOON_EXPORT void root_dock(const float2& pos, const float2& size) noexcept;
		OCTOON_EXPORT bool begin_dock(const char* label, bool* opened = nullptr, GuiWindowFlags extra_flags = 0, const float2& default_size = float2(-1, -1)) noexcept;
		OCTOON_EXPORT void end_dock() noexcept;
		OCTOON_EXPORT void set_dock_active() noexcept;

		OCTOON_EXPORT void render_frame_ex(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT bool begin_toolbar(const char* str_id, const float2& screen_pos, const float2& size) noexcept;
		OCTOON_EXPORT void end_toolbar() noexcept;
		OCTOON_EXPORT bool toolbar_button(GuiTextureID texture, const char* tooltip, bool selected = false, bool enabled = true) noexcept;

		namespace guizmo
		{
			enum Operation
			{
				Translate,
				Rotation,
				Scale
			};

			enum Mode
			{
				Local,
				World
			};

			OCTOON_EXPORT void SetDrawlist();
			OCTOON_EXPORT void BeginFrame();
			OCTOON_EXPORT bool IsOver();
			OCTOON_EXPORT bool IsUsing();
			OCTOON_EXPORT void Enable(bool enable);

			OCTOON_EXPORT void SetRect(float x, float y, float width, float height);

			OCTOON_EXPORT void Manipulate(const float *view, const float *projection, Operation operation, Mode mode, float *matrix, float *deltaMatrix = 0, float *snap = 0, float *localBounds = NULL, float *boundsSnap = NULL);
		};
	}
}

#endif