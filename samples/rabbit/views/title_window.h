#ifndef RABBIT_TITLE_WINDOW_H_
#define RABBIT_TITLE_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QtGui/qevent.h>

#include "rabbit_behaviour.h"

namespace rabbit
{
	class TitleWindow final : public QWidget
	{
		Q_OBJECT
	public:
		TitleWindow(QWidget* parent, const std::shared_ptr<rabbit::RabbitProfile>& profile) noexcept;
		~TitleWindow() noexcept;

	public Q_SLOTS:
		void showSmall() noexcept;
		void showMaxRestore() noexcept;
		void showSettingPlane() noexcept;
		void showVipPlane() noexcept;

		void loginSuccess() noexcept;
		void loginOut() noexcept;

	Q_SIGNALS:
		void settingSignal();
		void vipSignal();

	private:
		void mousePressEvent(QMouseEvent* e) noexcept override;
		void mouseReleaseEvent(QMouseEvent* e) noexcept override;
		void mouseMoveEvent(QMouseEvent* e) noexcept override;

	public:
		bool maxNormal_;
		bool allowMove_;

		QSize size_;

		QPoint startPos_;
		QPoint clickPos_;

		QIcon maxPix_;
		QIcon restorePix_;
		QIcon vipPix_;
		QIcon vipOnPix_;

		QHBoxLayout layout_;

		QLabel titleLabel_;

		QToolButton minimizeButton_;
		QToolButton maximizeButton_;
		QToolButton closeButton_;
		QToolButton settingButton_;
		QToolButton vipButton_;
		QPushButton logoButton_;

		std::shared_ptr<rabbit::RabbitProfile> profile_;
	};
}

#endif