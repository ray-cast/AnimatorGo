#ifndef RABBIT_MAIN_WINDOW_H
#define RABBIT_MAIN_WINDOW_H

#include "view_window.h"
#include "title_window.h"
#include "tool_window.h"
#include "hide_window.h"
#include "setting_window.h"
#include "color_dialog.h"
#include "rabbit_profile.h"
#include "splash_screen.h"
#include "sun_window.h"
#include "environment_window.h"
#include "login_window.h"
#include "info_window.h"
#include "record_window.h"
#include "material_window.h"

#include <fstream>
#include <octoon/game_listener.h>

namespace rabbit
{
	class MainListener : public octoon::GameListener
	{
	public:
		MainListener(SplashScreen* splash, const std::string& path);
		void onMessage(std::string_view message) noexcept override;

	public:
		SplashScreen* splash_;
		std::ofstream stream_;
	};

	class MainWindow final : public QFrame
	{
		Q_OBJECT
	public:
		MainWindow(SplashScreen* splash) noexcept(false);
		~MainWindow();

	private Q_SLOTS:
		void onHideToolBarSignal() noexcept;
		void onShowToolbarSignal() noexcept;
		bool onPlaySignal(bool enable) noexcept;
		bool onResetSignal() noexcept;
		void onLeftSignal() noexcept;
		void onRightSignal() noexcept;
		void onImportSignal() noexcept;
		bool onRecordSignal(bool enable) noexcept;
		void onScreenShotSignal() noexcept;
		bool onOfflineModeSignal(bool enable) noexcept;
		bool onImportHdriSignal(bool enable) noexcept;
		void onSettingSignal() noexcept;
		void onCleanupSignal() noexcept;
		void onProfileSignal(const rabbit::RabbitProfile& profile) noexcept;
		void onSunSignal() noexcept;
		void onEnvironmentSignal() noexcept;
		void onVipSignal() noexcept;
		void onMaterialSignal() noexcept;

		void onResizeSignal(QResizeEvent* e) noexcept;
		void onMousePressSignal(QMouseEvent* event) noexcept;
		void onMouseMoveSignal(QMouseEvent* event) noexcept;
		void onMouseReleaseSignal(QMouseEvent* event) noexcept;
		void onMouseDoubleClickSignal(QMouseEvent* event) noexcept;
		void onWheelSignal(QWheelEvent* event) noexcept;
		void onDragEnterSignal(QDragEnterEvent* event) noexcept;
		void onDropSignal(QDropEvent* event) noexcept;
		void onShowSignal(QShowEvent* event) noexcept;
		void onUpdateSignal() noexcept;

		bool eventFilter(QObject* watched, QEvent* event);

		void keyPressEvent(QKeyEvent* event) noexcept override;
		void keyReleaseEvent(QKeyEvent* event) noexcept override;

	private:
		bool open(int w, int h) noexcept;

	private:
		bool init_flag;
		SplashScreen* splash_;

		octoon::GameAppPtr gameApp_;
		octoon::GameObjectPtr behaviour_;

		std::unique_ptr<QTimer> timer;
		std::shared_ptr<rabbit::RabbitProfile> profile_;
		std::shared_ptr<MainListener> listener_;

		std::unique_ptr<ToolWindow> toolBar_;
		std::unique_ptr<TitleWindow> titleBar_;
		std::unique_ptr<ViewWidget> viewPanel_;
		std::unique_ptr<HideBar> hideBar_;
		std::unique_ptr<SettingWindow> settingWindow_;
		std::unique_ptr<SunWindow> sunWindow_;
		std::unique_ptr<EnvironmentWindow> environmentWindow_;
		std::unique_ptr<LoginWindow> loginWindow_;
		std::unique_ptr<InfoWindow> infoWindow_;
		std::unique_ptr<RecordWindow> recordWindow_;
		std::unique_ptr<MaterialWindow> materialWindow_;

		std::unique_ptr<QHBoxLayout> mainLayout_;
		std::unique_ptr<QVBoxLayout> contextLayout_;
	};
}

#endif