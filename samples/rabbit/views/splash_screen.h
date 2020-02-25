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

		std::unique_ptr<QPushButton> logo_;
		std::unique_ptr<QWidget> widget_;
		std::unique_ptr<QLabel> englishName_;
		std::unique_ptr<QLabel> versionInfo_;
		std::unique_ptr<QLabel> copyrightInfo_;
		std::unique_ptr<QLabel> message_;
		std::unique_ptr<QHBoxLayout> hlayout_;
		std::unique_ptr<QVBoxLayout> vlayout_;
		std::unique_ptr<QVBoxLayout> mainLayout_;
	};
}

#endif