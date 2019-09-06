#include "setting_window.h"
#include <qlabel.h>
#include <qscrollbar.h>

SettingContextPlane::SettingContextPlane(QWidget* parent) noexcept
	: QWidget(parent)
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

	widgetItems_[0] = std::make_unique<SettingMainPlane>(scrollWidget_.get());
	widgetItems_[1] = std::make_unique<SettingMainPlane2>(scrollWidget_.get());
	widgetItems_[2] = std::make_unique<SettingMainPlane3>(scrollWidget_.get());

	gridLayout_ = std::make_unique<QVBoxLayout>(scrollWidget_.get());
	gridLayout_->addWidget(widgetItems_[0].get());
	gridLayout_->addWidget(widgetItems_[1].get());
	gridLayout_->addWidget(widgetItems_[2].get());

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

SettingContextPlane::~SettingContextPlane()
{
	for (auto& it : widgetItems_)
		it.reset();

	gridLayout_.reset();
	scrollWidget_.reset();
	scrollArea_.reset();
	layout_.reset();
}

SettingWindow::SettingWindow() noexcept
	: settingTitleWindow_(std::make_unique<SettingTitleWindow>(this))
	, settingContextPlane_(std::make_unique<SettingContextPlane>(this))
{
	this->setObjectName("settingWidget");
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setGeometry(178, 178, 284, 132);
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