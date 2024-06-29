#ifndef UNREAL_TOOL_DOCK_H_
#define UNREAL_TOOL_DOCK_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qlabel.h>
#include <qdialog.h>
#include <qscrollarea.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>
#include <qbuttongroup.h>
#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class ToolDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		ToolDock(const octoon::GameAppPtr& gameApp, const octoon::GameObjectPtr& behaviour, std::shared_ptr<UnrealProfile> profile) noexcept;
		~ToolDock() noexcept;

	private Q_SLOTS:
		void recordEvent() noexcept;
		void sunEvent() noexcept;
		void environmentEvent() noexcept;
		void cameraEvent() noexcept;
		void inspectorEvent() noexcept;

	Q_SIGNALS:
		void sunSignal();
		void recordSignal();
		void environmentSignal();
		void cameraSignal();
		void inspectorSignal();

	public:
		QToolButton* videoButton_;
		QToolButton* sunButton_;
		QToolButton* environmentButton_;
		QToolButton* cameraButton_;
		QToolButton* inspectorButton_;

		octoon::GameObjectPtr behaviour_;
		std::shared_ptr<UnrealProfile> profile_;
	};
}

#endif