#include "draggable_button.h"
#include <qapplication.h>
#include <qdrag.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmimedata.h>

namespace unreal
{
	DraggableButton::DraggableButton() noexcept
	{
	}

	DraggableButton::~DraggableButton() noexcept
	{
	}

	void
	DraggableButton::mouseMoveEvent(QMouseEvent* event)
	{
		if (event->buttons() & Qt::LeftButton)
		{
			QPoint length = event->pos() - startPos;
			if (length.manhattanLength() > QApplication::startDragDistance())
			{
				auto mimeData = new QMimeData;
				mimeData->setData("object/dof", "Automatic");

				auto drag = new QDrag(this);
				drag->setMimeData(mimeData);
				drag->setPixmap(QPixmap(":res/icons/target.png"));
				drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
				drag->exec(Qt::MoveAction);

				emit mouseMoveSignal();
			}
		}

		QToolButton::mouseMoveEvent(event);
	}

	void
	DraggableButton::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
			startPos = event->pos();

		QToolButton::mousePressEvent(event);
	}
}