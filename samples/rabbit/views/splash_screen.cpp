#include "splash_screen.h"

namespace rabbit
{
	SplashScreen::SplashScreen()
	{
		this->setFrameShape(Panel);
		this->setObjectName("splashScreen");
		this->setWindowFlags(Qt::FramelessWindowHint);
		this->setFixedSize(500, 300);

		logo_ = std::make_unique<QPushButton>(this);
		logo_->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/splash.png")));
		logo_->setIconSize(QSize(110, 110));
		logo_->setContentsMargins(0, 0, 0, 0);

		widget_ = std::make_unique<QWidget>(this);
		widget_->setFixedSize(310, 100);

		englishName_ = std::make_unique<QLabel>(widget_.get());
		englishName_->setText(u8"Rabbit Toolbox 2019");
		englishName_->setFont(QFont("Microsoft Yahei", 14, QFont::Normal));
		englishName_->setAlignment(Qt::AlignLeft);
		englishName_->setStyleSheet("color:#c78ced;");

		copyrightInfo_ = std::make_unique<QLabel>(widget_.get());
		copyrightInfo_->setText(u8"© 2019 Rabbit-Softwares. All Rights Reserved.");
		copyrightInfo_->setFont(QFont("Microsoft Yahei", 8, QFont::Normal));
		copyrightInfo_->setAlignment(Qt::AlignLeft);
		copyrightInfo_->setStyleSheet("color:#a5a5a5;");

		vlayout_ = std::make_unique<QVBoxLayout>(widget_.get());
		vlayout_->addWidget(englishName_.get());
		vlayout_->addWidget(copyrightInfo_.get());
		vlayout_->setContentsMargins(0, 0, 0, 10);

		hlayout_ = std::make_unique<QHBoxLayout>();
		hlayout_->addWidget(logo_.get(), 0, Qt::AlignRight);
		hlayout_->addWidget(widget_.get(), 0, Qt::AlignLeft);
		hlayout_->setContentsMargins(0, 0, 0, 0);
		hlayout_->setSpacing(24);

		versionInfo_ = std::make_unique<QLabel>(this);
		versionInfo_->setText(u8"Version: 1.0.0");
		versionInfo_->setFont(QFont("Microsoft Yahei", 8, QFont::Normal));
		versionInfo_->setAlignment(Qt::AlignRight);
		versionInfo_->setStyleSheet("color:#505050;");
		versionInfo_->setContentsMargins(0, 10, 10, 0);

		message_ = std::make_unique<QLabel>(this);
		message_->setText(u8"Loading...");
		message_->setFont(QFont("Microsoft Yahei", 8, QFont::Normal));
		message_->setAlignment(Qt::AlignLeft);
		message_->setStyleSheet("color:#505050;");
		message_->setContentsMargins(10, 0, 0, 10);
		message_->setFixedWidth(500);

		mainLayout_ = std::make_unique<QVBoxLayout>();
		mainLayout_->addWidget(versionInfo_.get(), 0, Qt::AlignRight | Qt::AlignTop);
		mainLayout_->addLayout(hlayout_.get());
		mainLayout_->addWidget(message_.get(), 0, Qt::AlignLeft | Qt::AlignBottom);
		mainLayout_->setContentsMargins(0, 0, 0, 0);

		this->setLayout(mainLayout_.get());
	}

	SplashScreen::~SplashScreen()
	{
		englishName_.reset();
		copyrightInfo_.reset();
		vlayout_.reset();
		widget_.reset();
		logo_.reset();
		hlayout_.reset();
	}
}