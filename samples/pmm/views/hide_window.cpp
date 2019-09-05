#include "hide_window.h"

HideBar::HideBar(QWidget* parent)
{
	hideButton = std::make_unique<QPushButton>(this);
	hideButton->setIcon(QIcon::fromTheme("res", QIcon(":res/icons/show.png")));
	hideButton->setToolTip(u8"ÏÔÊ¾¹¤¾ßÀ¸");
	hideButton->setStyleSheet("QPushButton:hover{background-color:rgb(0,122,204,255);}");

	layout_ = std::make_unique<QVBoxLayout>(this);
	layout_->addWidget(hideButton.get());
	layout_->addStretch();
	layout_->setSpacing(0);
	layout_->setContentsMargins(0, 0, 0, 0);

	this->hide();
	this->setFixedWidth(6);
	this->connect(hideButton.get(), SIGNAL(clicked()), this, SLOT(showEvent()));
	this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

HideBar::~HideBar()
{
}

void
HideBar::resizeEvent(QResizeEvent* e) noexcept
{
	hideButton->setFixedSize(width(), height());
}

void
HideBar::showEvent()
{
	emit showSignal();
}