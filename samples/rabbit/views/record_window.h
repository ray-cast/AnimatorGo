#ifndef RABBIT_RECORD_WINDOW_H_
#define RABBIT_RECORD_WINDOW_H_

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
	class RecordWindow final : public QWidget
	{
		Q_OBJECT
	public:
		RecordWindow(QWidget* parent, const octoon::GameObjectPtr& behaviour) noexcept;
		~RecordWindow() noexcept;

		virtual void showEvent(QShowEvent* event) override;

		void update();
		void repaint();

		void startRecord(QString fileName);
		void stopRecord();

	private Q_SLOTS:
		void closeEvent();
		void clickEvent();
		void select1Event(bool checked);
		void select2Event(bool checked);
		void speed1Event(bool checked);
		void speed2Event(bool checked);
		void speed3Event(bool checked);
		void speed4Event(bool checked);
		void startEvent(int);
		void endEvent(int);
		void timeEvent();

	public:
		std::unique_ptr<QLabel> title_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QLabel> mark_;
		std::unique_ptr<QToolButton> markButton_;
		std::unique_ptr<QLabel> quality_;
		std::unique_ptr<QLabel> videoRatio_;
		std::unique_ptr<QLabel> infomation_;
		std::unique_ptr<QLabel> frame_;
		std::unique_ptr<QLabel> animation_;
		std::unique_ptr<QLabel> summary_;
		std::unique_ptr<QLabel> currentFrame_;
		std::unique_ptr<QLabel> timeTotal_;
		std::unique_ptr<QButtonGroup> group_;
		std::unique_ptr<QButtonGroup> speedGroup_;
		std::unique_ptr<QToolButton> select1_;
		std::unique_ptr<QToolButton> select2_;
		std::unique_ptr<QToolButton> speed1_;
		std::unique_ptr<QToolButton> speed2_;
		std::unique_ptr<QToolButton> speed3_;
		std::unique_ptr<QToolButton> speed4_;
		std::unique_ptr<QToolButton> backButton_;
		std::unique_ptr<QLabel> startLabel_;
		std::unique_ptr<QLabel> endLabel_;
		std::unique_ptr<QSpinBox> start_;
		std::unique_ptr<QSpinBox> end_;
		std::unique_ptr<QHBoxLayout> layout_;
		std::unique_ptr<QHBoxLayout> selectLayout_;
		std::unique_ptr<QHBoxLayout> videoRatioLayout_;
		std::unique_ptr<QHBoxLayout> frameLayout_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
		std::unique_ptr<QTimer> timer_;
		octoon::GameObjectPtr behaviour_;
	};
}

#endif