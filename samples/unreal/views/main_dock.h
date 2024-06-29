#ifndef UNREAL_MAIN_DOCK_H_
#define UNREAL_MAIN_DOCK_H_

#include <qwidget.h>
#include <qtimer.h>
#include <qdrag.h>
#include <qmainwindow.h>
#include <QTranslator>

#include <octoon/octoon.h>
#include <octoon/game_listener.h>

#include "unreal_profile.h"
#include "unreal_behaviour.h"

#include "views/splash_screen.h"
#include "views/status_bar.h"
#include "views/toplevel_bar.h"
#include "views/view_dock.h"
#include "views/tool_dock.h"
#include "views/asset_browse_dock.h"
#include "views/light_dock.h"
#include "views/main_light_dock.h"
#include "views/environment_dock.h"
#include "views/model_dock.h"
#include "views/material_dock.h"
#include "views/texture_dock.h"
#include "views/record_dock.h"
#include "views/camera_dock.h"
#include "views/motion_dock.h"
#include "views/inspector_dock.h"

namespace unreal
{
	class MainDock final : public QMainWindow
	{
		Q_OBJECT
	public:
		MainDock(SplashScreen* splash) noexcept;
		~MainDock() noexcept;

		void restoreLayout() noexcept;
		void saveLayout() noexcept;

		void setTranslator(std::shared_ptr<QTranslator> translator);
		std::shared_ptr<QTranslator> getTranslator();

	private Q_SLOTS:
		void showEvent(QShowEvent* event) noexcept override;
		void onLanguageChanged(QString filename);

	private:
		void onSunSignal() noexcept;
		void onLightSignal() noexcept;
		void onRecordSignal() noexcept;
		void onEnvironmentSignal() noexcept;
		void onMaterialSignal() noexcept;
		void onTextureSignal() noexcept;
		void onModelSignal() noexcept;
		void onCameraSignal() noexcept;
		void onMotionSignal() noexcept;
		void onImportSignal() noexcept;
		void onInspectorSignal() noexcept;

		void open() noexcept(false);
		void update() noexcept;

		QDockWidget* assetDock() noexcept;
		QDockWidget* toolDock() noexcept;

	private:
		bool init_flag;
		SplashScreen* splash_;

		QTimer timer;

		octoon::GameAppPtr gameApp_;
		octoon::GameObjectPtr behaviour_;

		std::shared_ptr<UnrealProfile> profile_;
		std::shared_ptr<SplashListener> listener_;

		std::unique_ptr<StatusBar> statusBar_;
		std::unique_ptr<ToplevelBar> toplevelDock_;
		std::unique_ptr<ViewDock> viewDock_;
		std::unique_ptr<ToolDock> toolDock_;
		std::unique_ptr<AssetBrowseDock> assetBrowseDock_;
		std::unique_ptr<RecordDock> recordDock_;
		std::unique_ptr<LightDock> lightDock_;
		std::unique_ptr<MainLightDock> mainLightDock_;
		std::unique_ptr<EnvironmentDock> environmentDock_;
		std::unique_ptr<MaterialDock> materialDock_;
		std::unique_ptr<ModelDock> modelDock_;
		std::unique_ptr<TextureDock> textureDock_;
		std::unique_ptr<CameraDock> cameraDock_;
		std::unique_ptr<MotionDock> motionDock_;
		std::unique_ptr<InspectorDock> inspectorDock_;

		std::shared_ptr<QTranslator> translator_;
	};
}

#endif