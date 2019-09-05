#ifndef VIEW_WIDGET_H
#define VIEW_WIDGET_H

#include <qwidget.h>
#include <qtimer.h>
#include <qdrag.h>

#include <octoon/octoon.h>

class ViewWidget final : public QWidget
{
	Q_OBJECT
public:
	ViewWidget(QWidget* parent = nullptr);
	~ViewWidget();

private Q_SLOTS:
	void paintEvent(QPaintEvent* e) noexcept override;
	void resizeEvent(QResizeEvent* e) noexcept override;
	void mousePressEvent(QMouseEvent* event) noexcept override;
	void mouseMoveEvent(QMouseEvent* event) noexcept override;
	void mouseReleaseEvent(QMouseEvent* event) noexcept override;
	void mouseDoubleClickEvent(QMouseEvent* event) noexcept override;
	void wheelEvent(QWheelEvent* event) noexcept override;
	void dragEnterEvent(QDragEnterEvent* event) noexcept override;
	void dropEvent(QDropEvent* event) noexcept override;
	void showEvent(QShowEvent* event) noexcept override;
	void updateEvent() noexcept;

	virtual QPaintEngine* paintEngine() const noexcept override;

Q_SIGNALS:
	void paintSignal(QPaintEvent* e);
	void resizeSignal(QResizeEvent* e);
	void mousePressSignal(QMouseEvent* event);
	void mouseMoveSignal(QMouseEvent* event);
	void mouseReleaseSignal(QMouseEvent* event);
	void mouseDoubleClickSignal(QMouseEvent* event);
	void wheelSignal(QWheelEvent* event);
	void dragEnterSignal(QDragEnterEvent* event);
	void dropSignal(QDropEvent* event);
	void showSignal(QShowEvent* event);
	void updateSignal();

private:
	octoon::GameAppPtr gameApp_;
	octoon::GameObjectPtr behaviour_;

	std::unique_ptr<QTimer> timer;
};

#endif