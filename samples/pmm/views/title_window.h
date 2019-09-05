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
	void mousePressEvent(QMouseEvent* e) override;
	void mouseReleaseEvent(QMouseEvent* e) override;
	void mouseMoveEvent(QMouseEvent* e) override;

private:
	bool maxNormal_;
	bool allowMove_;

	QSize size_;

	QPoint startPos_;
	QPoint clickPos_;

	QIcon maxPix_;
	QIcon restorePix_;

	std::unique_ptr<QHBoxLayout> layout_;

	std::unique_ptr<QLabel> titleLabel_;

	std::unique_ptr<QToolButton> minimizeButton_;
	std::unique_ptr<QToolButton> maximizeButton_;
	std::unique_ptr<QToolButton> closeButton_;
	std::unique_ptr<QToolButton> settingButton_;
	std::unique_ptr<QPushButton> logoButton_;
};