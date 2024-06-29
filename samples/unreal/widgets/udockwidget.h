#ifndef UNREAL_UDOCKWIDGET_H_
#define UNREAL_UDOCKWIDGET_H_

#include <QDockWidget>
#include "utranslatable.h"

namespace unreal
{
	class UDockWidget : public QDockWidget, public UTranslatable
	{
		Q_OBJECT
	public:
		explicit UDockWidget(QWidget* parent = 0);
		~UDockWidget();

		bool eventFilter(QObject* watched, QEvent* event);
	};
}

#endif