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
#include <qtimer.h>
#include "rabbit_behaviour.h"
#include <octoon/game_object.h>

namespace rabbit
{
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
		std::unique_ptr<QHBoxLayout> layout_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QTimer> timer_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif