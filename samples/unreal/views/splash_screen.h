#ifndef UNREAL_SPLASH_SCREEN_H_
#define UNREAL_SPLASH_SCREEN_H_

#include <qlayout.h>
#include <qframe>
#include <qpushbutton.h>
#include <qlabel.h>
#include <fstream>
#include <octoon/game_listener.h>

namespace unreal
{
	class SplashScreen final : public QFrame
	{
		Q_OBJECT
	public:
		SplashScreen();
		~SplashScreen();

	private Q_SLOTS:
		void showEvent(QShowEvent* event) noexcept override;

	public:
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

	class SplashListener : public octoon::GameListener
	{
	public:
		SplashListener(SplashScreen* splash, const std::string& path);

		void onMessage(std::string_view message) noexcept override;

	public:
		SplashScreen* splash_;
		std::ofstream stream_;
	};
}

#endif