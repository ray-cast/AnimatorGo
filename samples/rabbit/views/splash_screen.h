#ifndef RABBIT_SPLASH_SCREEN_H_
#define RABBIT_SPLASH_SCREEN_H_

#include <qlayout.h>
#include <qframe>
#include <qpushbutton.h>
#include <qlabel.h>

namespace rabbit
{
	class SplashScreen final : public QFrame
	{
		Q_OBJECT
	public:
		SplashScreen();
		~SplashScreen();

		QPushButton* logo;
		QWidget* widget;
		QLabel* englishName;
		QLabel* versionInfo;
		QLabel* copyrightInfo;
		QLabel* message;
		QHBoxLayout* hlayout;
		QVBoxLayout* vlayout;
		QVBoxLayout* mainLayout;
	};
}

#endif