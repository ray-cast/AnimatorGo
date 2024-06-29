#ifndef UNREAL_UPANEL_H_
#define UNREAL_UPANEL_H_

#include <QWidget>
#include "utranslatable.h"

namespace unreal
{
	class UPanel : public QWidget, public UTranslatable
	{
		Q_OBJECT
	public:
		explicit UPanel(QWidget* parent = 0);
		~UPanel();
        
		bool eventFilter(QObject* watched, QEvent* event);
	};
}

#endif