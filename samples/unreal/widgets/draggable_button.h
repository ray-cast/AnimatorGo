#ifndef UNREAL_DRAGGABLE_BUTTON_H_
#define UNREAL_DRAGGABLE_BUTTON_H_

#include <qevent.h>
#include <qtoolbutton.h>

namespace unreal
{
	class DraggableButton final : public QToolButton
	{
		Q_OBJECT
	public:
		DraggableButton() noexcept;
		~DraggableButton() noexcept;

		void mouseMoveEvent(QMouseEvent* event) override;
		void mousePressEvent(QMouseEvent* event) override;

	Q_SIGNALS:
		void mouseMoveSignal();

	private:
		QPoint startPos;
	};
}

#endif