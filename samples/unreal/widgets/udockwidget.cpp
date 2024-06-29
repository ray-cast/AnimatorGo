#include "udockwidget.h"

namespace unreal
{
	UDockWidget::UDockWidget(QWidget* parent)
		: QDockWidget(parent)
	{
        this->installEventFilter(this);
	}
	UDockWidget::~UDockWidget()
	{
	}

	bool
	UDockWidget::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() == QEvent::LanguageChange)
			retranslate();
		return QDockWidget::eventFilter(watched, event);
	}
}
