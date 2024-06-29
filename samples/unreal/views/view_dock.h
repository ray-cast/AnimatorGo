#ifndef UNREAL_VIEW_DOCK_H
#define UNREAL_VIEW_DOCK_H

#include <qdockwidget.h>
#include <qtimer.h>
#include <qdrag.h>

#include "unreal_profile.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class ViewDock final : public QDockWidget
	{
		Q_OBJECT
	public:
		ViewDock(const octoon::GameAppPtr& gameApp, const octoon::GameObjectPtr& behaviour, const std::shared_ptr<UnrealProfile>& profile) noexcept;
		~ViewDock() noexcept;

	private Q_SLOTS:
		void paintEvent(QPaintEvent* e) noexcept override;
		void resizeEvent(QResizeEvent* e) noexcept override;
		void mousePressEvent(QMouseEvent* event) noexcept override;
		void mouseMoveEvent(QMouseEvent* event) noexcept override;
		void mouseReleaseEvent(QMouseEvent* event) noexcept override;
		void mouseDoubleClickEvent(QMouseEvent* event) noexcept override;
		void keyPressEvent(QKeyEvent* event) noexcept override;
		void keyReleaseEvent(QKeyEvent* event) noexcept override;
		void wheelEvent(QWheelEvent* event) noexcept override;
		void dragEnterEvent(QDragEnterEvent* event) noexcept override;
		void dragMoveEvent(QDragMoveEvent *event) noexcept override;
		void dropEvent(QDropEvent* event) noexcept override;

		bool eventFilter(QObject* watched, QEvent* event);

		virtual QPaintEngine* paintEngine() const noexcept override;

	private:
		octoon::input::InputKey::Code KeyCodetoInputKey(int key) noexcept;

	private:
		octoon::GameAppPtr gameApp_;
		octoon::GameObjectPtr behaviour_;

		std::shared_ptr<unreal::UnrealProfile> profile_;
	};
}

#endif