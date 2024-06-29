#ifndef UNREAL_ASSET_BROWSE_DOCK_H_
#define UNREAL_ASSET_BROWSE_DOCK_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qscrollarea.h>
#include <qbuttongroup.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>
#include "unreal_profile.h"
#include "unreal_behaviour.h"
#include "setting_window.h"

namespace unreal
{
	class AssetBrowseDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		AssetBrowseDock(const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept;
		~AssetBrowseDock() noexcept;

		void closeEvent(QCloseEvent* event);

	private Q_SLOTS:
		void lightEvent() noexcept;
		void materialEvent() noexcept;
		void textureEvent() noexcept;
		void modelEvent() noexcept;
		void motionEvent() noexcept;
		void settingsEvent() noexcept;
		void importEvent() noexcept;

	Q_SIGNALS:
		void lightSignal();
		void modelSignal();
		void materialSignal();
		void textureSignal();
		void motionSignal();
		void importSignal();

	public:
		QToolButton* materialButton_;
		QToolButton* modelButton_;
		QToolButton* motionButton_;
		QToolButton* textureButton_;
		QToolButton* lightButton_;
		QToolButton* importButton_;
		QToolButton* settingsButton_;
		QButtonGroup* buttonGroup_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif