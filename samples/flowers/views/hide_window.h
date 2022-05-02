#ifndef RABBIT_HIDE_WINDOW_H_
#define RABBIT_HIDE_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qpushbutton.h>
#include <QtGui/qevent.h>
#include <qdockwidget.h>

namespace rabbit
{
	class HideBar final : public QWidget
	{
		Q_OBJECT
	public:
		HideBar(QWidget* parent);
		~HideBar();

		void resizeEvent(QResizeEvent* e) noexcept override;

	public Q_SLOTS:
		void showEvent();

	Q_SIGNALS:
		void showSignal();

	private:
		std::unique_ptr<QPushButton> hideButton_;
		std::unique_ptr<QVBoxLayout> layout_;
	};
}

#endif