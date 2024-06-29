#ifndef UNREAL_SPOILER_H_
#define UNREAL_SPOILER_H_

#include <qframe.h>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <qtoolbutton.h>
#include <qgridlayout.h>

namespace unreal
{
	class Spoiler final : public QWidget
	{
		Q_OBJECT
	public:
		explicit Spoiler(const QString& title = "", const int animationDuration = 100, QWidget* parent = 0);
		void setContentLayout(QLayout& contentLayout);

	public:
		QGridLayout mainLayout;
		QToolButton toggleButton;
		QParallelAnimationGroup toggleAnimation;
		QScrollArea contentArea;
		int animationDuration{ 300 };
	};
}

#endif