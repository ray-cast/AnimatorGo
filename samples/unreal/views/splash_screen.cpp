#include "splash_screen.h"
#include <qapplication.h>
#include <qscreen.h>

#include "unreal_version.h"

namespace unreal
{
	SplashScreen::SplashScreen()
	{
		this->setFrameShape(Panel);
		this->setObjectName("SplashWindow");
		this->setWindowFlags(Qt::FramelessWindowHint);

		logo = new QPushButton(this);
		logo->setObjectName("logo");

		widget = new QWidget(this);
		widget->setObjectName("text");

		englishName = new QLabel(widget);
		englishName->setObjectName("title");
		englishName->setText(tr("AnimatorGo Lite 2022"));
		englishName->setAlignment(Qt::AlignLeft);
		
		copyrightInfo = new QLabel(widget);
		copyrightInfo->setObjectName("copyright");
		copyrightInfo->setText(tr("2022 AnimatorGo-Softwares (R). All Rights Reserved."));
		copyrightInfo->setAlignment(Qt::AlignLeft);

		auto splitLine = new QFrame;
		splitLine->setObjectName("VLine");
		splitLine->setLineWidth(1);
		splitLine->setFixedHeight(90);
		splitLine->setFrameShape(QFrame::VLine);
		splitLine->setFrameShadow(QFrame::Plain);
		splitLine->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
		splitLine->setContentsMargins(0, 0, 0, 0);

		vlayout = new QVBoxLayout(widget);
		vlayout->addWidget(englishName);
		vlayout->addWidget(copyrightInfo);
		vlayout->setContentsMargins(0, 12, 0, 0);

		hlayout = new QHBoxLayout();
		hlayout->addStretch(0);
		hlayout->addWidget(logo, 0, Qt::AlignRight);
		hlayout->addSpacing(4);
		hlayout->addWidget(splitLine);
		hlayout->addSpacing(2);
		hlayout->addWidget(widget, 0, Qt::AlignLeft);
		hlayout->addStretch(0);
		hlayout->setContentsMargins(0, 0, 0, 0);
		hlayout->setSpacing(8);

		versionInfo = new QLabel(this);
		versionInfo->setObjectName("version");
		versionInfo->setText(tr("Version: ") + QString::fromStdString(UNREAL_VERSION));
		versionInfo->setAlignment(Qt::AlignRight);

		message = new QLabel(this);
		message->setObjectName("message");
		message->setText(tr("Loading..."));
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
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
		QRect rect = QApplication::primaryScreen()->geometry();
#else
		int currentScreen = QApplication::desktop()->screenNumber(this);
		QRect rect = QGuiApplication::screens().at(currentScreen)->geometry();
#endif
		
		this->move((rect.width() - this->width()) / 2, (rect.height() - this->height()) / 2);
	}

	SplashListener::SplashListener(SplashScreen* splash, const std::string& path)
		: splash_(splash)
		, stream_(path)
	{
	}

	void
	SplashListener::onMessage(std::string_view message) noexcept
	{
		if (splash_)
		{
			splash_->message->setText(QString::fromStdString(std::string(message)));
			splash_->repaint();
		}

		if (stream_.good())
		{
			stream_ << message;
			stream_ << "\n";
			stream_.flush();
		}
	}
}