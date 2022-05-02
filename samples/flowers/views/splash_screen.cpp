#include "splash_screen.h"
#include <qapplication.h>
#include <qscreen.h>

namespace flower
{
	SplashScreen::SplashScreen()
	{
		this->setFrameShape(Panel);
		this->setObjectName("SplashWindow");
		this->setWindowFlags(Qt::FramelessWindowHint);

		logo = new QPushButton(this);
		logo->setObjectName("logo");

		widget = new QWidget(this);
		widget->setFixedSize(320, 100);

		englishName = new QLabel(widget);
		englishName->setObjectName("title");
		englishName->setText(u8"Flowers Toolbox 2022");
		englishName->setAlignment(Qt::AlignLeft);

		copyrightInfo = new QLabel(widget);
		copyrightInfo->setObjectName("copyright");
		copyrightInfo->setText(u8"© 2022 Flowers-Softwares. All Rights Reserved.");
		copyrightInfo->setAlignment(Qt::AlignLeft);

		vlayout = new QVBoxLayout(widget);
		vlayout->addWidget(englishName);
		vlayout->addWidget(copyrightInfo);
		vlayout->setContentsMargins(0, 0, 0, 10);

		hlayout = new QHBoxLayout();
		hlayout->addWidget(logo, 0, Qt::AlignRight);
		hlayout->addWidget(widget, 0, Qt::AlignLeft);
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(20);

		versionInfo = new QLabel(this);
		versionInfo->setObjectName("version");
		versionInfo->setText(u8"Version: 1.0.0");
		versionInfo->setAlignment(Qt::AlignRight);

		message = new QLabel(this);
		message->setObjectName("message");
		message->setText(u8"Loading...");
		message->setAlignment(Qt::AlignLeft);

		mainLayout = new QVBoxLayout();
		mainLayout->addWidget(versionInfo, 0, Qt::AlignRight | Qt::AlignTop);
		mainLayout->addLayout(hlayout);
		mainLayout->addWidget(message, 0, Qt::AlignLeft | Qt::AlignBottom);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		this->setLayout(mainLayout);
	}

	SplashScreen::~SplashScreen()
	{
	}

	void
	SplashScreen::showEvent(QShowEvent* event) noexcept
	{
		// int currentScreen = QApplication::desktop()->screenNumber(this);
		// FIXME
		QRect rect = QGuiApplication::screens().at(0)->geometry();
		this->move((rect.width() - this->width()) / 2, (rect.height() - this->height()) / 2);
	}
}