#include "hide_window.h"

namespace rabbit
{
	HideBar::HideBar(QWidget* parent)
	{
		hideButton_ = std::make_unique<QPushButton>(this);
		hideButton_->setObjectName("show");
		hideButton_->setToolTip(u8"œ‘ æπ§æﬂ¿∏");

		layout_ = std::make_unique<QVBoxLayout>(this);
		layout_->addWidget(hideButton_.get());
		layout_->addStretch();
		layout_->setSpacing(0);
		layout_->setContentsMargins(0, 0, 0, 0);

		this->hide();
		this->setObjectName("showWidget");
		this->connect(hideButton_.get(), SIGNAL(clicked()), this, SLOT(showEvent()));
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	}

	HideBar::~HideBar()
	{
	}

	void
	HideBar::resizeEvent(QResizeEvent* e) noexcept
	{
		hideButton_->setFixedSize(width(), height());
	}

	void
	HideBar::showEvent()
	{
		emit showSignal();
	}
}