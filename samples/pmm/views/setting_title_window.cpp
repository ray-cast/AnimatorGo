#include "setting_title_window.h"

SettingTitleWindow::SettingTitleWindow(QWidget* parent) noexcept
	: allowMove_(true)
{
	settingButton_ = std::make_unique<QPushButton>(this);
	settingButton_->setObjectName("setting");

	titleLabel_ = std::make_unique<QLabel>(this);
	titleLabel_->setObjectName("title");
	titleLabel_->setText(u8"…Ë÷√");

	closeButton_ = std::make_unique<QToolButton>(this);
	closeButton_->setObjectName("close");
	closeButton_->setToolTip(u8"πÿ±’");

	layout_ = std::make_unique<QHBoxLayout>(this);
	layout_->setObjectName("titleLayout");
	layout_->addWidget(settingButton_.get());
	layout_->addWidget(titleLabel_.get());
	layout_->addWidget(closeButton_.get());

	layout_->setSpacing(0);
	layout_->insertStretch(2, 550);
	layout_->setContentsMargins(10, 0, 10, 0);

	this->connect(closeButton_.get(), SIGNAL(clicked()), this, SLOT(closeThis()));

	this->setObjectName("settingTitle");
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

SettingTitleWindow::~SettingTitleWindow() noexcept
{
}

void
SettingTitleWindow::closeThis()
{
	emit closeSignal();
}

void
SettingTitleWindow::mousePressEvent(QMouseEvent* e) noexcept
{
	allowMove_ = true;
	startPos_ = e->globalPos();
	clickPos_ = mapToParent(e->pos());
}

void
SettingTitleWindow::mouseReleaseEvent(QMouseEvent* e) noexcept
{
	allowMove_ = false;
}

void
SettingTitleWindow::mouseMoveEvent(QMouseEvent* e) noexcept
{
	if (allowMove_)
		parentWidget()->move(e->globalPos() - clickPos_);
}