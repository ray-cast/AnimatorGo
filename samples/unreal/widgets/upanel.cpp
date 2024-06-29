#include "upanel.h"

namespace unreal
{
	UPanel::UPanel(QWidget* parent)
		: QWidget(parent)
	{
        this->installEventFilter(this);
	}
	UPanel::~UPanel()
	{
	}
    bool
	UPanel::eventFilter(QObject* watched, QEvent* event)
	{
		if (event->type() == QEvent::LanguageChange)
			retranslate();
		return QWidget::eventFilter(watched, event);
	}
}
