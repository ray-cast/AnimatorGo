#include <qapplication.h>
#include <qdesktopwidget.h>

#include "views/main_window.h"

int main(int argc, char *argv[])
{
	QFile styleSheet(":res/qss/default.qss");

	if (styleSheet.open(QIODevice::ReadOnly))
	{
		QApplication app(argc, argv);
		app.setStyleSheet(styleSheet.readAll());

		MainWindow w;
		w.show();
		w.move((QApplication::desktop()->width() - w.width()) / 2, (QApplication::desktop()->height() - w.height()) / 2);

		return app.exec();
	}
	else
	{
		qWarning("Can't open the style sheet file.");
		return 0;
	}
}