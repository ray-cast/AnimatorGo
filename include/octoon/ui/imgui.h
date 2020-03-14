#ifndef OCTOON_IMGUI_H_
#define OCTOON_IMGUI_H_

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace imgui
	{
		using float2 = math::float2;
		using float3 = math::float3;
		using float4 = math::float4;

		struct GuiCol
		{
			enum Type
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

		OCTOON_EXPORT void newFrame() noexcept;

		OCTOON_EXPORT void showUserGuide() noexcept;
		OCTOON_EXPORT void showStyleEditor(GuiStyle* style = nullptr) noexcept;
		OCTOON_EXPORT void showTestWindow() noexcept;
		OCTOON_EXPORT void showMetricsWindow(bool* isOpened = nullptr) noexcept;

		OCTOON_EXPORT bool begin(const char* name, bool* isOpened = nullptr, GuiWindowFlags flags = 0) noexcept;
		OCTOON_EXPORT bool begin(const char* name, bool* isOpened, const float2& size_on_first_use, float bg_alpha = -1.0f, GuiWindowFlags flags = 0) noexcept;

		OCTOON_EXPORT bool beginChild(const char* str_id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool beginChild(GuiID id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool beginChildFrame(GuiID id, const float2& size, GuiWindowFlags extraFlags = 0) noexcept;

		OCTOON_EXPORT void end() noexcept;
		OCTOON_EXPORT void endChild() noexcept;
		OCTOON_EXPORT void endChildFrame() noexcept;

		OCTOON_EXPORT void beginGroup() noexcept;
		OCTOON_EXPORT void endGroup() noexcept;

		OCTOON_EXPORT bool beginMainMenuBar() noexcept;
		OCTOON_EXPORT bool beginMenuBar() noexcept;
		OCTOON_EXPORT bool beginMenu(const char* label, bool enabled = true) noexcept;

		OCTOON_EXPORT bool menuItem(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) noexcept;
		OCTOON_EXPORT bool menuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) noexcept;

		OCTOON_EXPORT void endMainMenuBar() noexcept;
		OCTOON_EXPORT void endMenuBar() noexcept;
		OCTOON_EXPORT void endMenu() noexcept;

		OCTOON_EXPORT void beginTooltip() noexcept;

		OCTOON_EXPORT void setTooltip(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void setTooltipV(const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT void endTooltip() noexcept;

		OCTOON_EXPORT bool beginPopup(const char* str_id) noexcept;
		OCTOON_EXPORT bool beginPopupModal(const char* name, bool* isOpened = nullptr, GuiWindowFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool beginPopupContextItem(const char* str_id, int mouse_button = 1) noexcept;
		OCTOON_EXPORT bool beginPopupContextWindow(const char* str_id = nullptr, int mouse_button = 1, bool also_over_items = true) noexcept;
		OCTOON_EXPORT bool beginPopupContextVoid(const char* str_id = nullptr, int mouse_button = 1) noexcept;

		OCTOON_EXPORT void openPopup(const char* str_id) noexcept;
		OCTOON_EXPORT void closeCurrentPopup() noexcept;

		OCTOON_EXPORT void endPopup() noexcept;

		OCTOON_EXPORT void setStyle(const GuiStyle& newStyle) noexcept;
		OCTOON_EXPORT const GuiStyle& getStyleDefault() noexcept;

		OCTOON_EXPORT void setWindowPos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowSize(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowCollapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowFocus() noexcept;
		OCTOON_EXPORT void setWindowPos(const char* name, const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowSize(const char* name, const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowCollapsed(const char* name, bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setWindowFocus(const char* name) noexcept;
		OCTOON_EXPORT void setWindowFontScale(float scale) noexcept;

		OCTOON_EXPORT void setNextWindowPos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setNextWindowPosCenter(GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setNextWindowSize(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setNextWindowSizeConstraints(const float2& size_min, const float2& size_max) noexcept;
		OCTOON_EXPORT void setNextWindowContentSize(const float2& size) noexcept;
		OCTOON_EXPORT void setNextWindowContentWidth(float width) noexcept;
		OCTOON_EXPORT void setNextWindowCollapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT void setNextWindowFocus() noexcept;

		OCTOON_EXPORT void setCursorPosX(float x) noexcept;
		OCTOON_EXPORT void setCursorPosY(float y) noexcept;
		OCTOON_EXPORT void setCursorPos(const float2& local_pos) noexcept;
		OCTOON_EXPORT void setCursorScreenPos(const float2& pos) noexcept;

		OCTOON_EXPORT void setKeyboardFocusHere(int offset = 0) noexcept;

		OCTOON_EXPORT bool isWindowHovered() noexcept;
		OCTOON_EXPORT bool isWindowFocused() noexcept;
		OCTOON_EXPORT bool isWindowCollapsed() noexcept;

		OCTOON_EXPORT bool isRootWindowFocused() noexcept;
		OCTOON_EXPORT bool isRootWindowOrAnyChildFocused() noexcept;
		OCTOON_EXPORT bool isRootWindowOrAnyChildHovered() noexcept;

		OCTOON_EXPORT bool isRectVisible(const float2& size) noexcept;

		OCTOON_EXPORT bool isPosHoveringAnyWindow(const float2& pos) noexcept;

		OCTOON_EXPORT bool isKeyDown(int key_index) noexcept;
		OCTOON_EXPORT bool isKeyPressed(int key_index, bool repeat = true) noexcept;
		OCTOON_EXPORT bool isKeyReleased(int key_index) noexcept;

		OCTOON_EXPORT bool isMouseDown(int button) noexcept;
		OCTOON_EXPORT bool isMouseClicked(int button, bool repeat = false) noexcept;
		OCTOON_EXPORT bool isMouseDoubleClicked(int button) noexcept;
		OCTOON_EXPORT bool isMouseReleased(int button) noexcept;
		OCTOON_EXPORT bool isMouseHoveringWindow() noexcept;
		OCTOON_EXPORT bool isMouseHoveringAnyWindow() noexcept;
		OCTOON_EXPORT bool isMouseHoveringRect(const float2& min, const float2& max, bool clip = true) noexcept;
		OCTOON_EXPORT bool isMouseDragging(int button = 0, float lock_threshold = -1.0f) noexcept;

		OCTOON_EXPORT float getContentRegionAvailWidth() noexcept;

		OCTOON_EXPORT float getWindowWidth() noexcept;
		OCTOON_EXPORT float getWindowHeight() noexcept;
		OCTOON_EXPORT float getWindowContentRegionWidth() noexcept;

		OCTOON_EXPORT float getCursorPosX() noexcept;
		OCTOON_EXPORT float getCursorPosY() noexcept;

		OCTOON_EXPORT float2 getCursorPos() noexcept;
		OCTOON_EXPORT float2 getCursorStartPos() noexcept;
		OCTOON_EXPORT float2 getCursorScreenPos() noexcept;

		OCTOON_EXPORT float2 getMousePos() noexcept;
		OCTOON_EXPORT float2 getMousePosOnOpeningCurrentPopup() noexcept;
		OCTOON_EXPORT float2 getMouseDragDelta(int button = 0, float lock_threshold = -1.0f) noexcept;

		OCTOON_EXPORT float2 getWindowPos() noexcept;
		OCTOON_EXPORT float2 getWindowSize() noexcept;
		OCTOON_EXPORT float2 getWindowContentRegionMin() noexcept;
		OCTOON_EXPORT float2 getWindowContentRegionMax() noexcept;

		OCTOON_EXPORT float2 getContentRegionMax() noexcept;
		OCTOON_EXPORT float2 getContentRegionAvail() noexcept;

		OCTOON_EXPORT float2 getDisplaySize() noexcept;

		OCTOON_EXPORT void resetMouseDragDelta(int button = 0) noexcept;

		OCTOON_EXPORT void captureKeyboardFromApp(bool capture = true) noexcept;
		OCTOON_EXPORT void captureMouseFromApp(bool capture = true) noexcept;

		OCTOON_EXPORT float getScrollX() noexcept;
		OCTOON_EXPORT float getScrollY() noexcept;
		OCTOON_EXPORT float getScrollMaxX() noexcept;
		OCTOON_EXPORT float getScrollMaxY() noexcept;

		OCTOON_EXPORT void  setScrollX(float scroll_x) noexcept;
		OCTOON_EXPORT void  setScrollY(float scroll_y) noexcept;
		OCTOON_EXPORT void  setScrollHere(float center_y_ratio = 0.5f) noexcept;
		OCTOON_EXPORT void  setScrollFromPosY(float pos_y, float center_y_ratio = 0.5f) noexcept;

		OCTOON_EXPORT void          pushStyleColor(GuiCol::Type idx, const float4& col) noexcept;
		OCTOON_EXPORT void          popStyleColor(int count = 1) noexcept;
		OCTOON_EXPORT void          pushStyleVar(GuiStyleVar idx, float val) noexcept;
		OCTOON_EXPORT void          pushStyleVar(GuiStyleVar idx, const float2& val) noexcept;
		OCTOON_EXPORT void          popStyleVar(int count = 1) noexcept;
		OCTOON_EXPORT float         getFontSize() noexcept;
		OCTOON_EXPORT float2        getFontTexUvWhitePixel() noexcept;
		OCTOON_EXPORT std::uint32_t getColorU32(GuiCol::Type idx, float alpha_mul = 1.0f) noexcept;
		OCTOON_EXPORT std::uint32_t getColorU32(const float4& col) noexcept;

		OCTOON_EXPORT void  pushItemWidth(float item_width) noexcept;
		OCTOON_EXPORT void  popItemWidth() noexcept;
		OCTOON_EXPORT float calcItemWidth() noexcept;
		OCTOON_EXPORT void  pushTextWrapPos(float wrap_pos_x = 0.0f) noexcept;
		OCTOON_EXPORT void  popTextWrapPos() noexcept;
		OCTOON_EXPORT void  pushAllowKeyboardFocus(bool v) noexcept;
		OCTOON_EXPORT void  popAllowKeyboardFocus() noexcept;
		OCTOON_EXPORT void  pushButtonRepeat(bool repeat) noexcept;
		OCTOON_EXPORT void  popButtonRepeat() noexcept;

		OCTOON_EXPORT void pushClipRect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept;
		OCTOON_EXPORT void popClipRect() noexcept;

		OCTOON_EXPORT void separator() noexcept;
		OCTOON_EXPORT void sameLine(float pos_x = 0.0f, float spacing_w = -1.0f) noexcept;
		OCTOON_EXPORT void newLine() noexcept;
		OCTOON_EXPORT void spacing() noexcept;
		OCTOON_EXPORT void dummy(const float2& size) noexcept;
		OCTOON_EXPORT void indent(float indent_w = 0.0f) noexcept;
		OCTOON_EXPORT void unindent(float indent_w = 0.0f) noexcept;

		OCTOON_EXPORT void alignFirstTextHeightToWidgets() noexcept;
		OCTOON_EXPORT float getTextLineHeight() noexcept;
		OCTOON_EXPORT float getTextLineHeightWithSpacing() noexcept;
		OCTOON_EXPORT float getItemsLineHeightWithSpacing() noexcept;

		OCTOON_EXPORT void columns(int count = 1, const char* id = nullptr, bool border = true) noexcept;

		OCTOON_EXPORT void pushID(const char* str_id_begin, const char* str_id_end) noexcept;
		OCTOON_EXPORT void pushID(const void* ptr_id) noexcept;
		OCTOON_EXPORT void pushID(int int_id) noexcept;
		OCTOON_EXPORT void popID() noexcept;

		OCTOON_EXPORT GuiID getID(const char* str_id) noexcept;
		OCTOON_EXPORT GuiID getID(const char* str_id_begin, const char* str_id_end) noexcept;
		OCTOON_EXPORT GuiID getID(const void* ptr_id) noexcept;

		OCTOON_EXPORT void text(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void textV(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void textColored(const float4& col, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void textColoredV(const float4& col, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void textDisabled(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void textDisabledV(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void textWrapped(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void textWrappedV(const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void textUnformatted(const char* text, const char* text_end = nullptr) noexcept;

		OCTOON_EXPORT void labelText(const char* label, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void labelTextV(const char* label, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void labelTextEx(const char* label, const char* fmt, ...) noexcept;
		OCTOON_EXPORT void labelTextExV(const char* label, const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT void bullet() noexcept;
		OCTOON_EXPORT void bulletText(const char* fmt, ...) noexcept;
		OCTOON_EXPORT void bulletTextV(const char* fmt, va_list args) noexcept;

		OCTOON_EXPORT bool button(const char* label, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool smallButton(const char* label) noexcept;
		OCTOON_EXPORT bool invisibleButton(const char* str_id, const float2& size) noexcept;
		OCTOON_EXPORT bool arrowButton(const char* str_id, GuiDir dir);

		OCTOON_EXPORT void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), const float4& tint_col = float4(1, 1, 1, 1), const float4& border_col = float4(0, 0, 0, 0)) noexcept;
		OCTOON_EXPORT bool imageButton(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), int frame_padding = -1, const float4& bg_col = float4(0, 0, 0, 0), const float4& tint_col = float4(1, 1, 1, 1)) noexcept;
		OCTOON_EXPORT bool imageButtonEx(GuiTextureID texture, const float2& size = float2(24, 24), const char* tooltip = nullptr, bool selected = false, bool enabled = true) noexcept;
		OCTOON_EXPORT bool imageButtonEx(GuiTextureID texture, const float2& size, bool enabled = true, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One) noexcept;
		OCTOON_EXPORT bool imageButtonAndLabel(const char* label, GuiTextureID texture, const float2& size, bool showLabel = true, bool selected = false, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One) noexcept;
		OCTOON_EXPORT bool imageButtonWithAspectAndLabel(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT void imageWithAspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0 = float2(0, 0), const float2& uv1 = float2(1, 1), const float4& tint_col = float4::One, const float4& border_col = float4::Zero) noexcept;

		OCTOON_EXPORT bool checkbox(const char* label, bool* v) noexcept;
		OCTOON_EXPORT bool checkboxFlags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept;

		OCTOON_EXPORT bool radioButton(const char* label, bool active) noexcept;
		OCTOON_EXPORT bool radioButton(const char* label, int* v, int v_button) noexcept;

		OCTOON_EXPORT bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool comboWithRevert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;

		OCTOON_EXPORT bool colorButton(const char* desc_id, const float4& col, GuiColorEditFlags flags = 0, float2 size = float2(0, 0)) noexcept;
		OCTOON_EXPORT bool colorEdit3(const char* label, float col[3]) noexcept;
		OCTOON_EXPORT bool colorEdit4(const char* label, float col[4], bool show_alpha = true) noexcept;

		OCTOON_EXPORT void plotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		OCTOON_EXPORT void plotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;
		OCTOON_EXPORT void plotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		OCTOON_EXPORT void plotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;

		OCTOON_EXPORT void progressBar(float fraction, const float2& size_arg = float2(-1, 0), const char* overlay = nullptr) noexcept;

		OCTOON_EXPORT bool dragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = nullptr, float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool dragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool dragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool dragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool dragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = nullptr) noexcept;
		OCTOON_EXPORT bool dragFloatnEx(const char* labels[], float* v, int components, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool dragIntnEx(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool dragUIntnEx(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* display_format = "%.0f") noexcept;

		OCTOON_EXPORT bool inputText(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT bool inputTextMultiline(const char* label, char* buf, size_t buf_size, const float2& size = float2::Zero, GuiInputTextFlags flags = 0) noexcept;
		OCTOON_EXPORT bool inputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputFloat2(const char* label, float v[2], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputFloat3(const char* label, float v[3], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputFloat4(const char* label, float v[4], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputInt(const char* label, int* v, int step = 1, int step_fast = 100, GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputInt2(const char* label, int v[2], GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputInt3(const char* label, int v[3], GuiInputTextFlags extraFlags = 0) noexcept;
		OCTOON_EXPORT bool inputInt4(const char* label, int v[4], GuiInputTextFlags extraFlags = 0) noexcept;

		OCTOON_EXPORT bool sliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloatWithRevert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat2WithRevert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat3WithRevert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		OCTOON_EXPORT bool sliderFloat4WithRevert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;

		OCTOON_EXPORT bool sliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f) noexcept;
		OCTOON_EXPORT bool sliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderIntv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderIntWithRevert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt2WithRevert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt3WithRevert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format = "%.0f") noexcept;
		OCTOON_EXPORT bool sliderInt4WithRevert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format = "%.0f") noexcept;

		OCTOON_EXPORT bool  treeNode(const char* label) noexcept;
		OCTOON_EXPORT bool  treeNode(const char* str_id, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  treeNode(const void* ptr_id, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  treeNodeV(const char* str_id, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  treeNodeV(const void* ptr_id, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  treeNodeEx(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		OCTOON_EXPORT bool  treeNodeEx(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  treeNodeEx(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		OCTOON_EXPORT bool  treeNodeExV(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT bool  treeNodeExV(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		OCTOON_EXPORT void  treePush(const char* str_id = nullptr) noexcept;
		OCTOON_EXPORT void  treePush(const void* ptr_id = nullptr) noexcept;
		OCTOON_EXPORT void  treePop() noexcept;
		OCTOON_EXPORT void  treeAdvanceToLabelPos() noexcept;
		OCTOON_EXPORT float getTreeNodeToLabelSpacing() noexcept;
		OCTOON_EXPORT void  setNextTreeNodeOpen(bool is__open, GuiSetCondFlags cond = 0) noexcept;
		OCTOON_EXPORT bool  collapsingHeader(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		OCTOON_EXPORT bool  collapsingHeader(const char* label, bool* isOpened, GuiTreeNodeFlags flags = 0) noexcept;

		OCTOON_EXPORT bool selectable(const char* label, bool selected = false, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool selectable(const char* label, bool* p_selected, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool listBox(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool listBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT bool listBoxHeader(const char* label, const float2& size = float2::Zero) noexcept;
		OCTOON_EXPORT bool listBoxHeader(const char* label, int items_count, int height_in_items = -1) noexcept;
		OCTOON_EXPORT void listBoxFooter() noexcept;

		OCTOON_EXPORT void value(const char* prefix, bool b) noexcept;
		OCTOON_EXPORT void value(const char* prefix, int v) noexcept;
		OCTOON_EXPORT void value(const char* prefix, unsigned int v) noexcept;
		OCTOON_EXPORT void value(const char* prefix, float v, const char* float_format = nullptr) noexcept;

		OCTOON_EXPORT float2 getItemRectMin() noexcept;
		OCTOON_EXPORT float2 getItemRectMax() noexcept;
		OCTOON_EXPORT float2 getItemRectSize() noexcept;
		OCTOON_EXPORT void setItemAllowOverlap() noexcept;

		OCTOON_EXPORT bool isItemHovered() noexcept;
		OCTOON_EXPORT bool isItemHoveredRect() noexcept;
		OCTOON_EXPORT bool isItemActive() noexcept;
		OCTOON_EXPORT bool isItemClicked(int mouse_button = 0) noexcept;
		OCTOON_EXPORT bool isItemVisible() noexcept;

		OCTOON_EXPORT bool isAnyItemActive() noexcept;
		OCTOON_EXPORT bool isAnyItemHovered() noexcept;

		OCTOON_EXPORT float getTime() noexcept;
		OCTOON_EXPORT int getFrameCount() noexcept;
		OCTOON_EXPORT const char* getStyleColorName(GuiCol::Type idx) noexcept;
		OCTOON_EXPORT float2 calcItemRectClosestPoint(const float2& pos, bool on_edge = false, float outward = +0.0f) noexcept;
		OCTOON_EXPORT float2 calcTextSize(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) noexcept;
		OCTOON_EXPORT void calcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept;

		OCTOON_EXPORT float4 colorConvertU32ToFloat4(std::uint32_t in) noexcept;
		OCTOON_EXPORT std::uint32_t colorConvertFloat4ToU32(const float4& in) noexcept;
		OCTOON_EXPORT void colorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept;
		OCTOON_EXPORT void colorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept;

		OCTOON_EXPORT bool colorPicker3(const char* label, float col[3], GuiColorEditFlags flags = 0);
		OCTOON_EXPORT bool colorPicker4(const char* label, float col[4], GuiColorEditFlags flags = 0, const float* ref_col = NULL);

		OCTOON_EXPORT void helpMarker(const char* text, const char* desc) noexcept;

		OCTOON_EXPORT void rootDock(const float2& pos, const float2& size) noexcept;
		OCTOON_EXPORT bool beginDock(const char* label, bool* opened = nullptr, GuiWindowFlags extra_flags = 0, const float2& default_size = float2(-1, -1)) noexcept;
		OCTOON_EXPORT void endDock() noexcept;
		OCTOON_EXPORT void setDockActive() noexcept;

		OCTOON_EXPORT void renderFrameEx(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness = 1.0f) noexcept;
		OCTOON_EXPORT bool beginToolbar(const char* str_id, const float2& screen_pos, const float2& size) noexcept;
		OCTOON_EXPORT void endToolbar() noexcept;
		OCTOON_EXPORT bool toolbarButton(GuiTextureID texture, const char* tooltip, bool selected = false, bool enabled = true) noexcept;
	}
}

#endif