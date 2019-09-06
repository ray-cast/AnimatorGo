#include "setting_window.h"
#include <qlabel.h>

SettingWindow::SettingWindow() noexcept
	: settingTitleWindow_(std::make_unique<SettingTitleWindow>(this))
{
	this->setObjectName("settingWidget");
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setGeometry(178, 178, 284, 132);
	this->setWindowModality(Qt::ApplicationModal);
	this->setMouseTracking(true);

	contextLayout_ = std::make_unique<QVBoxLayout>(this);
	contextLayout_->addWidget(settingTitleWindow_.get());
	contextLayout_->addWidget(this->setupContextWidget());
	contextLayout_->setMargin(0);
	contextLayout_->setSpacing(0);

	this->setFixedSize(650, 450);
	this->connect(settingTitleWindow_.get(), &SettingTitleWindow::closeSignal, this, &SettingWindow::closeEvent);
}

SettingWindow::~SettingWindow() noexcept
{
}

QWidget* 
SettingWindow::setupContextWidget() noexcept
{
	auto widget = new QWidget;

	QStringList strList{ tr(u8"÷˜√Ê∞Â"), tr(u8"‰÷»æ"), tr(u8" ”∆µ") };

	listWidget_ = std::make_unique<QListWidget>(widget);
	listWidget_->setFixedSize(160, 450);
	listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (std::size_t i = 0; i < strList.size(); i++)
	{
		listWidgetItems_[i] = std::make_unique<QListWidgetItem>();
		listWidgetItems_[i]->setText(strList[i]);
		listWidgetItems_[i]->setTextColor(QColor(255, 255, 255));
		listWidgetItems_[i]->setSizeHint(QSize(160, 30));
		listWidgetItems_[i]->setTextAlignment(Qt::AlignCenter);

		listWidget_->addItem(listWidgetItems_[i].get());
	}

	gridLayout_ = std::make_unique<QGridLayout>();

	for (std::size_t i = 0; i < strList.size(); i++)
	{
		QWidget* widget = new QWidget(scrollWidget_.get());

		QLabel* imageLabel = new QLabel(widget);
		QPixmap pixmap(":res/icons/logo.png");
		pixmap = pixmap.scaled(200, 200, Qt::KeepAspectRatio);
		imageLabel->setPixmap(pixmap);
		imageLabel->setStyleSheet("background: white;");
		imageLabel->setAlignment(Qt::AlignCenter);

		auto layout_ = new QHBoxLayout(widget);
		layout_->setObjectName("contextLayout");
		layout_->addWidget(imageLabel);
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);

		gridLayout_->addWidget(widget);
	}

	scrollWidget_ = std::make_unique<QWidget>();
	scrollWidget_->setLayout(gridLayout_.get());

	scrollArea_ = std::make_unique<QScrollArea>();
	scrollArea_->setFixedSize(490, 450);
	scrollArea_->setWidget(scrollWidget_.get());

	layout_ = std::make_unique<QHBoxLayout>(widget);
	layout_->setObjectName("contextLayout");
	layout_->addWidget(listWidget_.get());
	layout_->addWidget(scrollArea_.get());
	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);

	return widget;
}

void
SettingWindow::closeEvent()
{
	this->close();
}