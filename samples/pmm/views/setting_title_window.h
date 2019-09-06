#ifndef SETTING_TITLE_WINDOW_H_
#define SETTING_TITLE_WINDOW_H_

#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QtGui/qevent.h>

class SettingTitleWindow final : public QWidget
{
	Q_OBJECT
public:
	SettingTitleWindow(QWidget* parent) noexcept;
	~SettingTitleWindow() noexcept;

public Q_SLOTS:
	void closeThis();

Q_SIGNALS:
	void closeSignal();

private:
	void mousePressEvent(QMouseEvent* e) noexcept override;
	void mouseReleaseEvent(QMouseEvent* e) noexcept override;
	void mouseMoveEvent(QMouseEvent* e) noexcept override;

private:
	bool allowMove_;

	QSize size_;

	QPoint startPos_;
	QPoint clickPos_;

	std::unique_ptr<QHBoxLayout> layout_;

	std::unique_ptr<QLabel> titleLabel_;

	std::unique_ptr<QToolButton> closeButton_;
	std::unique_ptr<QPushButton> settingButton_;
	std::unique_ptr<QPushButton> logoButton_;
};

#endif