#ifndef UNREAL_INFO_WINDOW_H_
#define UNREAL_INFO_WINDOW_H_

#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qaction.h>
#include <qevent.h>
#include <qvalidator.h>
#include <qstackedwidget.h>
#include <qpropertyanimation.h>
#include <qtimer.h>
#include <future>

#include "unreal_behaviour.h"

namespace unreal
{
	class InfoWindow final : public QFrame
	{
		Q_OBJECT
	public:
		InfoWindow(QWidget* widget, const std::shared_ptr<unreal::UnrealBehaviour>& behaviour) noexcept;
		~InfoWindow() noexcept;

	private:
		void mousePressEvent(QMouseEvent* e) noexcept override;
		void mouseReleaseEvent(QMouseEvent* e) noexcept override;
		void mouseMoveEvent(QMouseEvent* e) noexcept override;
		void showEvent(QShowEvent* event) noexcept override;

		bool eventFilter(QObject* watched, QEvent* event);

	public Q_SLOTS:
		void closeEvent();
		void loginOutEvent();
		void select1Event();
		void select2Event();
		void select3Event();
		void qrcodeClickEvent();
		void timeEvent();
		void tradeEvent();
		void refreshEvent();

	Q_SIGNALS:
		void loginOut();

	public:
		bool allowMove_;
		bool isRoratingWindow_;
		std::uint32_t nextPageIndex_;
		std::string trade_no_;

		QPoint startPos_;
		QPoint clickPos_;
		QWidget* parent_;

		std::uint32_t tradeType_;
		std::unique_ptr<QTimer> timer_;
		std::unique_ptr<QTimer> tradeTimer_;
		std::future<nlohmann::json> future_;

		std::unique_ptr<QPropertyAnimation> closeAnimation_;

		std::unique_ptr<QLabel> vipLabel_;
		std::unique_ptr<QLabel> payLabel_;
		std::unique_ptr<QPushButton> countLabel_;
		std::unique_ptr<QPushButton> qrcodeButton_;
		std::unique_ptr<QPushButton> logo_;
		std::unique_ptr<QPushButton> username_;
		std::unique_ptr<QToolButton> closeButton_;
		std::unique_ptr<QToolButton> loginOutButton_;
		std::unique_ptr<QToolButton> selectButton1_;
		std::unique_ptr<QToolButton> selectButton2_;
		std::unique_ptr<QToolButton> selectButton3_;
		std::unique_ptr<QPushButton> vipButton_;
		std::unique_ptr<QPushButton> licenseButton_;

		std::unique_ptr<QHBoxLayout> titleLayout_;
		std::unique_ptr<QHBoxLayout> selectLayout_;
		std::unique_ptr<QHBoxLayout> lisenseLayout_;
		std::unique_ptr<QVBoxLayout> layout_;
		std::shared_ptr<unreal::UnrealBehaviour> behaviour_;
	};
}

#endif