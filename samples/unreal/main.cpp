#include <memory>

#include <qapplication.h>
#include <qqmlcontext.h>
#include <qquickview.h>
#include <qqmlapplicationengine.h>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QDir>

#include "bindings/application.h"

#include "views/main_dock.h"
#include "views/splash_screen.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"

std::shared_ptr<spdlog::logger> create_logger()
{
	try
	{
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::warn);
		console_sink->set_pattern("[%H:%M:%S %z] [%l] [%n] [%^---%L---%$] [thread %t] %v");

		auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(QDir::homePath().toStdString() + "/.animator/logs/log.txt", 2, 30);
		file_sink->set_level(spdlog::level::trace);
		file_sink->set_pattern("[%H:%M:%S %z] [%l] [%n] [%^---%L---%$] [thread %t] %v");

		std::shared_ptr<spdlog::logger> logger(new spdlog::logger("logger", { console_sink, file_sink }));
		logger->set_level(spdlog::level::trace);

		return logger;
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log init failed: " << ex.what() << std::endl;
		return nullptr;
	}
}

int main(int argc, char *argv[])
{
	auto logger = create_logger();
	spdlog::set_default_logger(logger);
	spdlog::info("Application started");
	
#ifdef _WINDOWS_
	::SetConsoleOutputCP(CP_UTF8);
#endif

	QFile styleSheet(":res/qss/white.qss");

	if (styleSheet.open(QIODevice::ReadOnly))
	{
		QApplication app(argc, argv);
		app.setStyleSheet(styleSheet.readAll());

#ifdef _WINDOWS_
		auto filepath = app.applicationDirPath();
		::SetCurrentDirectoryA(filepath.toStdString().c_str());
#endif
		// Load translation files
		std::shared_ptr<QTranslator> qtTranslator = std::make_shared<QTranslator>();
		QString local = QLocale::languageToString(QLocale::system().language());
		spdlog::info("Current machine locale: " + local.toStdString());

		if (local == "Chinese")
			qtTranslator->load("zh_CN.qm", ":res/languages/");
		else if (local == "English")
			qtTranslator->load("en_US.qm", ":res/languages/");
		else
			qtTranslator->load("en_US.qm", ":res/languages/");
		app.installTranslator(qtTranslator.get());
		
		// make splash screen
		auto splash = std::make_unique<unreal::SplashScreen>();
		splash->show();
		app.processEvents();

		unreal::MainDock w(splash.get());
		w.setTranslator(qtTranslator);
		w.show();
		w.activateWindow();

		splash.reset();

		app.exec();
}
	else
	{
		qWarning("Can't open the style sheet file.");
		spdlog::error("Can't open the style sheet file.");
	}
	spdlog::info("Application exited");
	return 0;
}