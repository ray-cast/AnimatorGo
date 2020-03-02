#ifndef RABBIT_MATERIAL_WINDOW_H_
#define RABBIT_MATERIAL_WINDOW_H_

#include <qwidget>
#include <qdialog.h>
#include <qboxlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <optional>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistwidget.h>
#include <qcheckbox.h>
#include <qtimer.h>
#include "rabbit_behaviour.h"
#include <octoon/game_object.h>

namespace rabbit
{
	class ListDragWidget final : public QListWidget
	{
		Q_OBJECT
	public:
		ListDragWidget();
		ListDragWidget(QWidget* parent);
		~ListDragWidget();

	protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

		void dragEnterEvent(QDragEnterEvent* event);
		void dragMoveEvent(QDragMoveEvent* event);
		void dropEvent(QDropEvent* event);
	private:
		QPoint dragPoint_;
	};

	class MaterialWindow final : public QWidget
	{
		Q_OBJECT
	public:
		MaterialWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept;
		~MaterialWindow() noexcept;

		virtual void showEvent(QShowEvent* event) override;

	private Q_SLOTS:
		void closeEvent();

	private:
		std::unique_ptr<QLabel> title_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QHBoxLayout> titleLayout_;
		std::unique_ptr<QVBoxLayout> materialLayout_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QListWidget> listWidget_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif