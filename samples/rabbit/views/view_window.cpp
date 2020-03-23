#include "views/view_window.h"
#include "rabbit_behaviour.h"

namespace rabbit
{
	ViewWidget::ViewWidget(QWidget* parent, const std::shared_ptr<rabbit::RabbitProfile>& profile) noexcept
		: QWidget(parent)
		, timer(std::make_unique<QTimer>(this))
	{
		this->setAttribute(Qt::WA_PaintOnScreen, true);
		this->setObjectName("viewWidget");
		this->setMouseTracking(true);
		this->setUpdatesEnabled(false);
		this->setAcceptDrops(true);
		this->setMinimumSize(profile->canvasModule->width, profile->canvasModule->height);
		this->setFocusPolicy(Qt::StrongFocus);

		this->connect(timer.get(), SIGNAL(timeout()), this, SLOT(updateEvent()));

		timer->start();
	}

	ViewWidget::~ViewWidget() noexcept
	{
	}

	void 
	ViewWidget::paintEvent(QPaintEvent* e) noexcept
	{
		emit paintSignal(e);
	}

	void 
	ViewWidget::resizeEvent(QResizeEvent* e) noexcept
	{
		emit resizeSignal(e);
	}

	void 
	ViewWidget::mouseMoveEvent(QMouseEvent* e) noexcept
	{
		emit mouseMoveSignal(e);
	}

	void 
	ViewWidget::mousePressEvent(QMouseEvent* e) noexcept
	{
		emit mousePressSignal(e);
	}

	void 
	ViewWidget::mouseReleaseEvent(QMouseEvent* e) noexcept
	{
		emit mouseReleaseSignal(e);
	}

	void 
	ViewWidget::mouseDoubleClickEvent(QMouseEvent* e) noexcept
	{
		emit mouseDoubleClickSignal(e);
	}

	void 
	ViewWidget::wheelEvent(QWheelEvent* e) noexcept
	{
		emit wheelSignal(e);
	}

	void
	ViewWidget::showEvent(QShowEvent* e) noexcept
	{
		emit showSignal(e);
	}

	void
	ViewWidget::dragEnterEvent(QDragEnterEvent* e) noexcept
	{
		emit dragEnterSignal(e);
	}

	void
	ViewWidget::dropEvent(QDropEvent* e) noexcept
	{
		emit dropSignal(e);
	}

	void
	ViewWidget::updateEvent() noexcept
	{
		emit updateSignal();
	}

	QPaintEngine*
	ViewWidget::paintEngine() const noexcept
	{
		return nullptr;
	}
}