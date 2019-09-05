#include <qapplication.h>
#include <qlayout.h>
#include <qstyle>
#include <qwidget>
#include <qtoolButton.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <QtGui/qevent.h>

class TitleBar final : public QWidget
{
	Q_OBJECT
public:
	TitleBar(QWidget* parent);
	~TitleBar();

public Q_SLOTS:
	void showSmall();
	void showMaxRestore();

private:
	void mousePressEvent(QMouseEvent* me) override;
	void mouseReleaseEvent(QMouseEvent* me) override;
	void mouseMoveEvent(QMouseEvent* me) override;

private:
	QToolButton* minimizeButton;
	QToolButton* maximizeButton;
	QToolButton* closeButton;
	QToolButton* settingButton;
	QPushButton* logoButton;
	QIcon restorePix, maxPix;
	bool maxNormal;
	bool allowMove;
	QPoint startPos;
	QPoint clickPos;
};