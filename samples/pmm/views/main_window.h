#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view_window.h"
#include "title_window.h"
#include "tool_window.h"
#include "hide_window.h"
#include "setting_window.h"

class MainWindow final : public QFrame
{
public:
	MainWindow();
	~MainWindow();

private Q_SLOTS:
	void onHideToolBarSignal() noexcept;
	void onShowToolbarSignal() noexcept;
	bool onPlaySignal(bool enable) noexcept;
	void onImportSignal() noexcept;
	bool onRecordSignal(bool enable) noexcept;
	void onScreenShotSignal() noexcept;
	bool onOfflineModeSignal(bool enable) noexcept;
	void onImportHdriSignal() noexcept;
	void onSettingSignal() noexcept;

	void onPaintSignal(QPaintEvent* e) noexcept;
	void onResizeSignal(QResizeEvent* e) noexcept;
	void onMousePressSignal(QMouseEvent* event) noexcept;
	void onMouseMoveSignal(QMouseEvent* event) noexcept;
	void onMouseReleaseSignal(QMouseEvent* event) noexcept;
	void onMouseDoubleClickSignal(QMouseEvent* event) noexcept;
	void onKeyPressSignal(QKeyEvent* event) noexcept;
	void onKeyReleaseSignal(QKeyEvent* event) noexcept;
	void onWheelSignal(QWheelEvent* event) noexcept;
	void onDragEnterSignal(QDragEnterEvent* event) noexcept;
	void onDropSignal(QDropEvent* event) noexcept;
	void onShowSignal(QShowEvent* event) noexcept;
	void onUpdateSignal() noexcept;

private:
	bool open(int w, int h) noexcept;

private:
	bool init_flag;

	octoon::GameAppPtr gameApp_;
	octoon::GameObjectPtr behaviour_;

	std::unique_ptr<QTimer> timer;

	std::unique_ptr<ToolBar> toolBar_;
	std::unique_ptr<TitleBar> titleBar_;
	std::unique_ptr<ViewWidget> viewPanel_;
	std::unique_ptr<HideBar> hideBar_;
	std::unique_ptr<SettingWindow> settingWindow_;

	std::unique_ptr<QHBoxLayout> mainLayout_;
	std::unique_ptr<QVBoxLayout> contextLayout_;
};

#endif