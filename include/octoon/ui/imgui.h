#ifndef _H_IMGUI_H_
#define _H_IMGUI_H_

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

		static void newFrame() noexcept;
		static void render() noexcept;
		static void shutdown() noexcept;

		static void showUserGuide() noexcept;
		static void showStyleEditor(GuiStyle* style = nullptr) noexcept;
		static void showTestWindow(bool* isOpened = nullptr) noexcept;
		static void	showMetricsWindow(bool* isOpened = nullptr) noexcept;

		static bool begin(const char* name, bool* isOpened = nullptr, GuiWindowFlags flags = 0) noexcept;
		static bool begin(const char* name, bool* isOpened, const float2& size_on_first_use, float bg_alpha = -1.0f, GuiWindowFlags flags = 0) noexcept;

		static bool beginChild(const char* str_id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		static bool beginChild(GuiID id, const float2& size = float2::Zero, bool border = false, GuiWindowFlags extraFlags = 0) noexcept;
		static bool beginChildFrame(GuiID id, const float2& size, GuiWindowFlags extraFlags = 0) noexcept;

		static void end() noexcept;
		static void endChild() noexcept;
		static void endChildFrame() noexcept;

		static void setStyle(const GuiStyle& newStyle) noexcept;
		static const GuiStyle& getStyleDefault() noexcept;

		static void setWindowPos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowSize(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowCollapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowFocus() noexcept;
		static void setWindowPos(const char* name, const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowSize(const char* name, const float2& size, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowCollapsed(const char* name, bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		static void setWindowFocus(const char* name) noexcept;
		static void setWindowFontScale(float scale) noexcept;

		static void setNextWindowPos(const float2& pos, GuiSetCondFlags cond = 0) noexcept;
		static void setNextWindowPosCenter(GuiSetCondFlags cond = 0) noexcept;
		static void setNextWindowSize(const float2& size, GuiSetCondFlags cond = 0) noexcept;
		static void setNextWindowSizeConstraints(const float2& size_min, const float2& size_max) noexcept;
		static void setNextWindowContentSize(const float2& size) noexcept;
		static void setNextWindowContentWidth(float width) noexcept;
		static void setNextWindowCollapsed(bool collapsed, GuiSetCondFlags cond = 0) noexcept;
		static void setNextWindowFocus() noexcept;

		static void setCursorPosX(float x) noexcept;
		static void setCursorPosY(float y) noexcept;
		static void setCursorPos(const float2& local_pos) noexcept;
		static void setCursorScreenPos(const float2& pos) noexcept;
		static void setCursorIcon(GuiMouseCursor type) noexcept;

		static void setKeyboardFocusHere(int offset = 0) noexcept;

		static bool isWindowHovered() noexcept;
		static bool isWindowFocused() noexcept;
		static bool isWindowCollapsed() noexcept;

		static bool isRootWindowFocused() noexcept;
		static bool isRootWindowOrAnyChildFocused() noexcept;
		static bool isRootWindowOrAnyChildHovered() noexcept;

		static bool isRectVisible(const float2& size) noexcept;

		static bool isPosHoveringAnyWindow(const float2& pos) noexcept;

		static bool isKeyDown(int key_index) noexcept;
		static bool isKeyPressed(int key_index, bool repeat = true) noexcept;
		static bool isKeyReleased(int key_index) noexcept;

		static bool isMouseDown(int button) noexcept;
		static bool isMouseClicked(int button, bool repeat = false) noexcept;
		static bool isMouseDoubleClicked(int button) noexcept;
		static bool isMouseReleased(int button) noexcept;
		static bool isMouseHoveringWindow() noexcept;
		static bool isMouseHoveringAnyWindow() noexcept;
		static bool isMouseHoveringRect(const float2& min, const float2& max, bool clip = true) noexcept;
		static bool isMouseDragging(int button = 0, float lock_threshold = -1.0f) noexcept;

		static float getContentRegionAvailWidth() noexcept;

		static float getWindowWidth() noexcept;
		static float getWindowHeight() noexcept;
		static float getWindowContentRegionWidth() noexcept;

		static float getCursorPosX() noexcept;
		static float getCursorPosY() noexcept;

		static float2 getCursorPos() noexcept;
		static float2 getCursorStartPos() noexcept;
		static float2 getCursorScreenPos() noexcept;

		static float2 getMousePos() noexcept;
		static float2 getMousePosOnOpeningCurrentPopup() noexcept;
		static float2 getMouseDragDelta(int button = 0, float lock_threshold = -1.0f) noexcept;

		static float2 getWindowPos() noexcept;
		static float2 getWindowSize() noexcept;
		static float2 getWindowContentRegionMin() noexcept;
		static float2 getWindowContentRegionMax() noexcept;

		static float2 getContentRegionMax() noexcept;
		static float2 getContentRegionAvail() noexcept;

		static float2 getDisplaySize() noexcept;

		static void resetMouseDragDelta(int button = 0) noexcept;

		static void captureKeyboardFromApp(bool capture = true) noexcept;
		static void captureMouseFromApp(bool capture = true) noexcept;

		static float getScrollX() noexcept;
		static float getScrollY() noexcept;
		static float getScrollMaxX() noexcept;
		static float getScrollMaxY() noexcept;

		static void  setScrollX(float scroll_x) noexcept;
		static void  setScrollY(float scroll_y) noexcept;
		static void  setScrollHere(float center_y_ratio = 0.5f) noexcept;
		static void  setScrollFromPosY(float pos_y, float center_y_ratio = 0.5f) noexcept;

		static void          pushStyleColor(GuiCol idx, const float4& col) noexcept;
		static void          popStyleColor(int count = 1) noexcept;
		static void          pushStyleVar(GuiStyleVar idx, float val) noexcept;
		static void          pushStyleVar(GuiStyleVar idx, const float2& val) noexcept;
		static void          popStyleVar(int count = 1) noexcept;
		static float         getFontSize() noexcept;
		static float2        getFontTexUvWhitePixel() noexcept;
		static std::uint32_t getColorU32(GuiCol idx, float alpha_mul = 1.0f) noexcept;
		static std::uint32_t getColorU32(const float4& col) noexcept;

		static void  pushItemWidth(float item_width) noexcept;
		static void  popItemWidth() noexcept;
		static float calcItemWidth() noexcept;
		static void  pushTextWrapPos(float wrap_pos_x = 0.0f) noexcept;
		static void  popTextWrapPos() noexcept;
		static void  pushAllowKeyboardFocus(bool v) noexcept;
		static void  popAllowKeyboardFocus() noexcept;
		static void  pushButtonRepeat(bool repeat) noexcept;
		static void  popButtonRepeat() noexcept;

		static void pushClipRect(const float2& clip_rect_min, const float2& clip_rect_max, bool intersect_with_current_clip_rect) noexcept;
		static void popClipRect() noexcept;

		static void separator() noexcept;
		static void sameLine(float pos_x = 0.0f, float spacing_w = -1.0f) noexcept;
		static void newLine() noexcept;
		static void spacing() noexcept;
		static void dummy(const float2& size) noexcept;
		static void indent(float indent_w = 0.0f) noexcept;
		static void unindent(float indent_w = 0.0f) noexcept;
		static void beginGroup() noexcept;
		static void endGroup() noexcept;

		static void alignFirstTextHeightToWidgets() noexcept;
		static float getTextLineHeight() noexcept;
		static float getTextLineHeightWithSpacing() noexcept;
		static float getItemsLineHeightWithSpacing() noexcept;

		static void columns(int count = 1, const char* id = nullptr, bool border = true) noexcept;

		static void pushID(const char* str_id_begin, const char* str_id_end) noexcept;
		static void pushID(const void* ptr_id) noexcept;
		static void pushID(int int_id) noexcept;
		static void popID() noexcept;

		static GuiID getID(const char* str_id) noexcept;
		static GuiID getID(const char* str_id_begin, const char* str_id_end) noexcept;
		static GuiID getID(const void* ptr_id) noexcept;

		static void text(const char* fmt, ...) noexcept;
		static void textV(const char* fmt, va_list args) noexcept;
		static void textColored(const float4& col, const char* fmt, ...) noexcept;
		static void textColoredV(const float4& col, const char* fmt, va_list args) noexcept;
		static void textDisabled(const char* fmt, ...) noexcept;
		static void textDisabledV(const char* fmt, va_list args) noexcept;
		static void textWrapped(const char* fmt, ...) noexcept;
		static void textWrappedV(const char* fmt, va_list args) noexcept;
		static void textUnformatted(const char* text, const char* text_end = nullptr) noexcept;
		static void labelText(const char* label, const char* fmt, ...) noexcept;
		static void labelTextV(const char* label, const char* fmt, va_list args) noexcept;
		static void bullet() noexcept;
		static void bulletText(const char* fmt, ...) noexcept;
		static void bulletTextV(const char* fmt, va_list args) noexcept;
		static bool button(const char* label, const float2& size = float2::Zero) noexcept;
		static bool smallButton(const char* label) noexcept;
		static bool invisibleButton(const char* str_id, const float2& size) noexcept;
		static void image(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), const float4& tint_col = float4(1, 1, 1, 1), const float4& border_col = float4(0, 0, 0, 0)) noexcept;
		static bool imageButton(GuiTextureID user_texture_id, const float2& size, const float2& uv0 = float2::Zero, const float2& uv1 = float2(1, 1), int frame_padding = -1, const float4& bg_col = float4(0, 0, 0, 0), const float4& tint_col = float4(1, 1, 1, 1)) noexcept;
		static bool checkbox(const char* label, bool* v) noexcept;
		static bool checkboxFlags(const char* label, unsigned int* flags, unsigned int flags_value) noexcept;
		static bool radioButton(const char* label, bool active) noexcept;
		static bool radioButton(const char* label, int* v, int v_button) noexcept;
		static bool combo(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		static bool comboWithRevert(const char* label, const char* revert, int* current_item, int _default, const char** items, int items_count, int height_in_items = -1) noexcept;
		static bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items = -1) noexcept;
		static bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;
		static bool colorButton(const float4& col, bool small_height = false, bool outline_border = true) noexcept;
		static bool colorEdit3(const char* label, float col[3]) noexcept;
		static bool colorEdit4(const char* label, float col[4], bool show_alpha = true) noexcept;
		static void colorEditMode(GuiColorEditMode mode) noexcept;
		static void plotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		static void plotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;
		static void plotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero, int stride = sizeof(float)) noexcept;
		static void plotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = nullptr, float scale_min = FLT_MAX, float scale_max = FLT_MAX, const float2& graph_size = float2::Zero) noexcept;
		static void progressBar(float fraction, const float2& size_arg = float2(-1, 0), const char* overlay = nullptr) noexcept;

		static bool dragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool dragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool dragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool dragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool dragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = nullptr, float power = 1.0f) noexcept;
		static bool dragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		static bool dragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		static bool dragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		static bool dragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f") noexcept;
		static bool dragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = nullptr) noexcept;

		static bool inputText(const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0) noexcept;
		static bool inputTextMultiline(const char* label, char* buf, size_t buf_size, const float2& size = float2::Zero, GuiInputTextFlags flags = 0) noexcept;
		static bool inputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputFloat2(const char* label, float v[2], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputFloat3(const char* label, float v[3], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputFloat4(const char* label, float v[4], int decimal_precision = -1, GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputInt(const char* label, int* v, int step = 1, int step_fast = 100, GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputInt2(const char* label, int v[2], GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputInt3(const char* label, int v[3], GuiInputTextFlags extraFlags = 0) noexcept;
		static bool inputInt4(const char* label, int v[4], GuiInputTextFlags extraFlags = 0) noexcept;

		static bool sliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloatv(const char* label, const float2& size, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloatWithRevert(const char* label, const char* name, float* v, float _default, float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat2WithRevert(const char* label, const char* name, float v[2], const float _default[2], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat3WithRevert(const char* label, const char* name, float v[3], const float _default[3], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;
		static bool sliderFloat4WithRevert(const char* label, const char* name, float v[4], const float _default[4], float _min, float _max, const char* display_format = "%.3f", float power = 1.0f) noexcept;

		static bool sliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f) noexcept;
		static bool sliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		static bool sliderIntv(const char* label, const float2& size, int* v, int v_min, int v_max, const char* display_format = "%.0f") noexcept;
		static bool sliderIntWithRevert(const char* label, const char* name, int* v, int _default, int _min, int _max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt2WithRevert(const char* label, const char* name, int* v, const int _default[2], int _min, int _max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt3WithRevert(const char* label, const char* name, int* v, const int _default[3], int _min, int _max, const char* display_format = "%.0f") noexcept;
		static bool sliderInt4WithRevert(const char* label, const char* name, int* v, const int _default[4], int _min, int _max, const char* display_format = "%.0f") noexcept;

		static bool  treeNode(const char* label) noexcept;
		static bool  treeNode(const char* str_id, const char* fmt, ...) noexcept;
		static bool  treeNode(const void* ptr_id, const char* fmt, ...) noexcept;
		static bool  treeNodeV(const char* str_id, const char* fmt, va_list args) noexcept;
		static bool  treeNodeV(const void* ptr_id, const char* fmt, va_list args) noexcept;
		static bool  treeNodeEx(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		static bool  treeNodeEx(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		static bool  treeNodeEx(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, ...) noexcept;
		static bool  treeNodeExV(const char* str_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		static bool  treeNodeExV(const void* ptr_id, GuiTreeNodeFlags flags, const char* fmt, va_list args) noexcept;
		static void  treePush(const char* str_id = nullptr) noexcept;
		static void  treePush(const void* ptr_id = nullptr) noexcept;
		static void  treePop() noexcept;
		static void  treeAdvanceToLabelPos() noexcept;
		static float getTreeNodeToLabelSpacing() noexcept;
		static void  setNextTreeNodeOpen(bool is_open, GuiSetCondFlags cond = 0) noexcept;
		static bool  collapsingHeader(const char* label, GuiTreeNodeFlags flags = 0) noexcept;
		static bool  collapsingHeader(const char* label, bool* isOpened, GuiTreeNodeFlags flags = 0) noexcept;

		static bool selectable(const char* label, bool selected = false, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		static bool selectable(const char* label, bool* p_selected, GuiSelectableFlags flags = 0, const float2& size = float2::Zero) noexcept;
		static bool listBox(const char* label, int* current_item, const char** items, int items_count, int height_in_items = -1) noexcept;
		static bool listBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1) noexcept;
		static bool listBoxHeader(const char* label, const float2& size = float2::Zero) noexcept;
		static bool listBoxHeader(const char* label, int items_count, int height_in_items = -1) noexcept;
		static void listBoxFooter() noexcept;

		static void value(const char* prefix, bool b) noexcept;
		static void value(const char* prefix, int v) noexcept;
		static void value(const char* prefix, unsigned int v) noexcept;
		static void value(const char* prefix, float v, const char* float_format = nullptr) noexcept;
		static void valueColor(const char* prefix, const float4& v) noexcept;
		static void valueColor(const char* prefix, std::uint32_t v) noexcept;

		static void setTooltip(const char* fmt, ...) noexcept;
		static void setTooltipV(const char* fmt, va_list args) noexcept;
		static void beginTooltip() noexcept;
		static void endTooltip() noexcept;

		static bool beginMainMenuBar() noexcept;
		static bool beginMenuBar() noexcept;
		static bool beginMenu(const char* label, bool enabled = true) noexcept;

		static void endMainMenuBar() noexcept;
		static void endMenuBar() noexcept;
		static void endMenu() noexcept;

		static bool menuItem(const char* label, const char* shortcut = nullptr, bool selected = false, bool enabled = true) noexcept;
		static bool menuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true) noexcept;

		static void openPopup(const char* str_id) noexcept;
		static bool beginPopup(const char* str_id) noexcept;
		static bool beginPopupModal(const char* name, bool* isOpened = nullptr, GuiWindowFlags extraFlags = 0) noexcept;
		static bool beginPopupContextItem(const char* str_id, int mouse_button = 1) noexcept;
		static bool beginPopupContextWindow(bool also_over_items = true, const char* str_id = nullptr, int mouse_button = 1) noexcept;
		static bool beginPopupContextVoid(const char* str_id = nullptr, int mouse_button = 1) noexcept;
		static void endPopup() noexcept;
		static void closeCurrentPopup() noexcept;

		static bool isAnyItemActive() noexcept;
		static float2 getItemRectMin() noexcept;
		static float2 getItemRectMax() noexcept;
		static float2 getItemRectSize() noexcept;
		static void setItemAllowOverlap() noexcept;

		static bool isItemHovered() noexcept;
		static bool isItemHoveredRect() noexcept;
		static bool isItemActive() noexcept;
		static bool isItemClicked(int mouse_button = 0) noexcept;
		static bool isItemVisible() noexcept;

		static bool isAnyItemHovered() noexcept;

		static float getTime() noexcept;
		static int getFrameCount() noexcept;
		static const char* getStyleColName(GuiCol idx) noexcept;
		static float2 calcItemRectClosestPoint(const float2& pos, bool on_edge = false, float outward = +0.0f) noexcept;
		static float2 calcTextSize(const char* text, const char* text_end = nullptr, bool hide_text_after_double_hash = false, float wrap_width = -1.0f) noexcept;
		static void calcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end) noexcept;

		static float4 colorConvertU32ToFloat4(std::uint32_t in) noexcept;
		static std::uint32_t colorConvertFloat4ToU32(const float4& in) noexcept;
		static void colorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v) noexcept;
		static void colorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b) noexcept;

		static bool colorPicker3(const char* label, float col[3], const float2& size = float2(160.f, 150.f), float hueSize = 12.0f, float crossHairSize = 7.0f) noexcept;
		static bool colorPicker3WithRevert(const char* label, const char* name, float col[3], const float default[3], const float2& size = float2(160.f, 150.f), float hueSize = 12.0f, float crossHairSize = 7.0f) noexcept;

		static void helpMarker(const char* text, const char* desc) noexcept;

		static void rootDock(const float2& pos, const float2& size);
		static bool beginDock(const char* label, bool* opened = nullptr, GuiWindowFlags extra_flags = 0, const float2& default_size = float2(-1, -1));
		static void endDock();
		static void setDockActive();

		static bool dragFloatNEx(const char* labels[], float* v, int components, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
		static bool dragIntNEx(const char* labels[], int* v, int components, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
		static bool dragUIntNEx(const char* labels[], unsigned int* v, int components, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* display_format = "%.0f");
		static void renderFrameEx(const float2& p_min, const float2& p_max, bool border, float rounding, float thickness = 1.0f);
		static bool beginToolbar(const char* str_id, const float2& screen_pos, const float2& size);
		static void endToolbar();
		static bool toolbarButton(GuiTextureID texture, const char* tooltip, bool selected = false, bool enabled = true);
		static bool imageButtonEx(GuiTextureID texture, const float2& size = float2(24, 24), const char* tooltip = nullptr, bool selected = false, bool enabled = true);
		static bool imageButtonEx(GuiTextureID texture, const float2& size, bool enabled = true, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One);
		static bool imageButtonAndLabel(const char* label, GuiTextureID texture, const float2& size, bool showLabel = true, bool selected = false, const float2& uv0 = float2::Zero, const float2& uv1 = float2::One, int frame_padding = -1, const float4& bg_col = float4::Zero, const float4& tint_col = float4::One);
		static bool imageButtonWithAspectAndLabel(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0, const float2& uv1, bool selected, bool* edit_label, const char* label, char* buf, size_t buf_size, GuiInputTextFlags flags = 0);
		static void imageWithAspect(GuiTextureID texture, const float2& texture_size, const float2& size, const float2& uv0 = float2(0, 0), const float2& uv1 = float2(1, 1), const float4& tint_col = float4::One, const float4& border_col = float4::Zero);
		static void labelTextEx(const char* label, const char* fmt, ...);
		static void labelTextExV(const char* label, const char* fmt, va_list args);
	}
}

#endif