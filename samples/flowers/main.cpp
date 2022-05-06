#include <qapplication.h>
#include <qqmlcontext.h>
#include <qquickview.h>
#include <qqmlapplicationengine.h>
#include <QFile>

#include "bindings/application.h"

#include "views/main_window.h"
#include "views/splash_screen.h"

int main(int argc, char *argv[])
{
#ifdef _WINDOWS_
	::SetConsoleOutputCP(CP_UTF8);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif

#if 0

	QGuiApplication app(argc, argv);

	auto applicationData = std::make_unique<flower::Application>();

	app.exec();

	return 0;
#else
	QFile styleSheet(":res/qss/default.qss");

	if (styleSheet.open(QIODevice::ReadOnly))
	{
		QApplication app(argc, argv);
		app.setStyleSheet(styleSheet.readAll());

		auto splash = std::make_unique<flower::SplashScreen>();
		splash->show();
		app.processEvents();

		flower::MainWindow w(splash.get());
		w.show();

		splash.reset();

		app.exec();
	}
	else
	{
		qWarning("Can't open the style sheet file.");
		return 0;
	}
#endif
}