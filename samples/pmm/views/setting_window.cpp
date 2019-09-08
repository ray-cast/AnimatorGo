#include "setting_window.h"
#include <qlabel.h>
#include <qscrollbar.h>

SettingContextPlane::SettingContextPlane(QWidget* parent, const std::shared_ptr<MysticLit::MysticLitProfile>& profile) noexcept
	: QWidget(parent)
	, profile_(profile)
{
	this->setObjectName("settingContext");

	QStringList strList{ tr(u8"÷˜√Ê∞Â"), tr(u8"‰÷»æ"), tr(u8" ”∆µ") };

	listWidget_ = std::make_unique<QListWidget>(this);
	listWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	for (std::size_t i = 0; i < strList.size(); i++)
	{
		listWidgetItems_[i] = std::make_unique<QListWidgetItem>();
		listWidgetItems_[i]->setText(strList[i]);
		listWidgetItems_[i]->setSizeHint(QSize(180, 30));
		listWidgetItems_[i]->setTextAlignment(Qt::AlignCenter);

		listWidget_->addItem(listWidgetItems_[i].get());
	}

	listWidgetItems_[0]->setSelected(true);

	scrollWidget_ = std::make_unique<QWidget>(this);
	scrollWidget_->setFixedWidth(490);
	scrollWidget_->setStyleSheet("background-color: rgb(40,40,40);");

	mainPlane_ = std::make_unique<SettingMainPlane>(scrollWidget_.get());
	mainPlane2_ = std::make_unique<SettingMainPlane2>(scrollWidget_.get());
	mainPlane3_ = std::make_unique<SettingMainPlane3>(scrollWidget_.get());

	gridLayout_ = std::make_unique<QVBoxLayout>(scrollWidget_.get());
	gridLayout_->addWidget(mainPlane_.get());
	gridLayout_->addWidget(mainPlane2_.get());
	gridLayout_->addWidget(mainPlane3_.get());

	scrollArea_ = std::make_unique<QScrollArea>(this);
	scrollArea_->setWidget(scrollWidget_.get());
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	layout_ = std::make_unique<QHBoxLayout>(this);
	layout_->addWidget(listWidget_.get());
	layout_->addWidget(scrollArea_.get());
	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);

	connect((const QObject*)scrollArea_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	connect(listWidget_.get(), SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));

	if (profile->canvasModule->width == 320 && profile->canvasModule->height == 240)
		mainPlane2_->resolutionCombo->setCurrentIndex(0);
	else if (profile->canvasModule->width == 720 && profile->canvasModule->height == 480)
		mainPlane2_->resolutionCombo->setCurrentIndex(1);
	else if (profile->canvasModule->width == 800 && profile->canvasModule->height == 480)
		mainPlane2_->resolutionCombo->setCurrentIndex(2);
	else if (profile->canvasModule->width == 1024 && profile->canvasModule->height == 576)
		mainPlane2_->resolutionCombo->setCurrentIndex(3);
	else if (profile->canvasModule->width == 1280 && profile->canvasModule->height == 720)
		mainPlane2_->resolutionCombo->setCurrentIndex(4);
	else if (profile->canvasModule->width == 1920 && profile->canvasModule->height == 1080)
		mainPlane2_->resolutionCombo->setCurrentIndex(5);

	if (profile->timeModule->recordFps == 24)
		mainPlane2_->speedCombo->setCurrentIndex(0);
	else if (profile->timeModule->recordFps == 30)
		mainPlane2_->speedCombo->setCurrentIndex(1);
	else if (profile->timeModule->recordFps == 60)
		mainPlane2_->speedCombo->setCurrentIndex(2);
}

SettingContextPlane::~SettingContextPlane()
{
	switch (mainPlane2_->resolutionCombo->currentIndex())
	{
		case 0:
		{
			profile_->canvasModule->width = 320;
			profile_->canvasModule->height = 240;
		}
		break;
		case 1:
		{
			profile_->canvasModule->width = 720;
			profile_->canvasModule->height = 480;
		}
		break;
		case 2:
		{
			profile_->canvasModule->width = 800;
			profile_->canvasModule->height = 480;
		}
		break;
		case 3:
		{
			profile_->canvasModule->width = 1024;
			profile_->canvasModule->height = 576;
		}
		break;
		case 4:
		{
			profile_->canvasModule->width = 1280;
			profile_->canvasModule->height = 720;
		}
		break;
		case 5:
		{
			profile_->canvasModule->width = 1920;
			profile_->canvasModule->height = 1080;
		}
		break;
	}

	switch (mainPlane2_->speedCombo->currentIndex())
	{
	case 0:
		profile_->timeModule->recordFps == 24;
	break;
	case 1:
		profile_->timeModule->recordFps == 30;
	break;
	case 2:
		profile_->timeModule->recordFps == 60;
	break;
	}

	mainPlane_.reset();
	mainPlane2_.reset();
	mainPlane3_.reset();
	gridLayout_.reset();
	scrollWidget_.reset();
	scrollArea_.reset();
	layout_.reset();
}

void
SettingContextPlane::valueChanged(int value)
{
	if (scrollArea_->widget()->layout()->count() != listWidget_->count())
		return;

	if (!m_sign)
	{
		for (std::size_t i = 0; i < listWidget_->count(); i++)
		{
			auto widget = scrollArea_->widget()->layout()->itemAt(i)->widget();
			if (!widget->visibleRegion().isEmpty())
			{
				listWidget_->item(i)->setSelected(true);
				return;
			}
			else
			{
				listWidget_->item(i)->setSelected(false);
			}
		}
	}

	m_sign = false;
}

void
SettingContextPlane::itemClicked(QListWidgetItem* item)
{
	if (scrollArea_->widget()->layout()->count() != listWidget_->count())
		return;

	m_sign = true;

	for (std::size_t i = 0; i < listWidget_->count(); i++)
	{
		if (item == listWidget_->item(i))
		{
			auto widget = scrollArea_->widget()->layout()->itemAt(i)->widget();
			scrollArea_->verticalScrollBar()->setSliderPosition(widget->pos().y());
		}
	}
}

SettingWindow::SettingWindow(const std::shared_ptr<MysticLit::MysticLitProfile>& profile) noexcept
	: settingTitleWindow_(std::make_unique<SettingTitleWindow>(this))
	, settingContextPlane_(std::make_unique<SettingContextPlane>(this, profile))
{
	this->setObjectName("settingWidget");
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setWindowModality(Qt::ApplicationModal);
	this->setMouseTracking(true);

	contextLayout_ = std::make_unique<QVBoxLayout>(this);
	contextLayout_->addWidget(settingTitleWindow_.get());
	contextLayout_->addWidget(settingContextPlane_.get());
	contextLayout_->setMargin(0);
	contextLayout_->setSpacing(0);

	this->setFixedSize(650, 450);
	this->connect(settingTitleWindow_.get(), &SettingTitleWindow::closeSignal, this, &SettingWindow::closeEvent);
}

SettingWindow::~SettingWindow() noexcept
{
	settingContextPlane_.reset();
	settingTitleWindow_.reset();
}

void
SettingWindow::closeEvent()
{
	this->close();
}